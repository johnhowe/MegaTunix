/*
 * Copyright (C) 2003 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute etc. this as long as the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

#include <config.h>
#include <dashboard.h>
#include <defines.h>
#include <enums.h>
#include <general_gui.h>
#include <gui_handlers.h>
#include <interrogate.h>
#include <math.h>
#include <structures.h>
#include <tabloader.h>
#include <widgetmgmt.h>

extern gboolean tips_in_use;
extern gint temp_units;
extern GdkColor black;
extern gint dbg_lvl;
GtkWidget *ms_ecu_revision_entry;
gboolean use_timestamps = FALSE;

GtkTextBuffer *textbuffer;

static struct DebugLevel dbglevels[] = {
{ "Interrogation", DEBUG_LEVEL, INTERROGATOR, INTERROGATOR_SHIFT, FALSE},
{ "OpenGL", DEBUG_LEVEL, OPENGL, OPENGL_SHIFT, FALSE},
{ "Conversions", DEBUG_LEVEL, CONVERSIONS, CONVERSIONS_SHIFT, FALSE},
{ "Serial Reads", DEBUG_LEVEL, SERIAL_RD, SERIAL_RD_SHIFT, FALSE},
{ "Serial Writes", DEBUG_LEVEL, SERIAL_WR, SERIAL_WR_SHIFT, FALSE},
{ "I/O Processing", DEBUG_LEVEL, IO_PROCESS, IO_PROCESS_SHIFT, FALSE},
{ "Threads", DEBUG_LEVEL, THREADS, THREADS_SHIFT, FALSE},
{ "Req Fuel", DEBUG_LEVEL, REQ_FUEL, REQ_FUEL_SHIFT, FALSE},
{ "Tabloader", DEBUG_LEVEL, TABLOADER, TABLOADER_SHIFT, FALSE},
{ "RealTime Maps", DEBUG_LEVEL, RTMLOADER, RTMLOADER_SHIFT, FALSE},
{ "Complex Math", DEBUG_LEVEL, COMPLEX_EXPR, COMPLEX_EXPR_SHIFT, FALSE},
{ "Critical Errors", DEBUG_LEVEL, CRITICAL, CRITICAL_SHIFT, TRUE},
};


/*!
 \brief build_general() Builds the general Tab for the MegaTunix gui which
 houses the butons for tooltips, temp scale selection and most importantly
 the Interrogation status windows and offline mode selectio buttons
 \param parent_frame (GtkWidget *) pointer to the parent frame
 */
void build_general(GtkWidget *parent_frame)
{
	extern GtkTooltips *tip;
	GtkWidget *vbox = NULL;
	GtkWidget *vbox2 = NULL;
	GtkWidget *sw = NULL;
	GtkWidget *view = NULL;
	GtkWidget *frame = NULL;
	GtkWidget *hbox = NULL;
	GtkWidget *hbox2 = NULL;
	GtkWidget *button = NULL;
	GtkWidget *cbutton = NULL;
	GtkWidget *table = NULL;
	GtkWidget *label = NULL;
	GtkWidget *entry = NULL;
	GtkWidget *ebox = NULL;
	gint i,j,k;
	gint shift = 0;
	gint mask = 0;
	GSList *group = NULL;
	gint num_levels = sizeof(dbglevels)/sizeof(dbglevels[0]);
	extern gchar * cluster_1_name;
	extern gchar * cluster_2_name;

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(parent_frame),vbox);

	hbox = gtk_hbox_new(TRUE,5);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);

	frame = gtk_frame_new("Context Sensitive Help");
	gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,TRUE,0);

	hbox2 = gtk_hbox_new(TRUE,0);
	gtk_container_add(GTK_CONTAINER(frame),hbox2);
	gtk_container_set_border_width(GTK_CONTAINER(hbox2),3);

	button = gtk_check_button_new_with_label("Use ToolTips");
	gtk_box_pack_start(GTK_BOX(hbox2),button,FALSE,FALSE,2);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),tips_in_use);
	g_object_set_data(G_OBJECT(button),"handler",GINT_TO_POINTER(TOOLTIPS_STATE));
	g_signal_connect (G_OBJECT(button), "toggled",
			G_CALLBACK (toggle_button_handler),
			NULL);

	frame = gtk_frame_new("Temperature Scale");
	gtk_box_pack_start(GTK_BOX(hbox),frame,FALSE,TRUE,0);

	table = gtk_table_new(1,5,TRUE);
	gtk_container_add(GTK_CONTAINER(frame),table);
	gtk_container_set_border_width(GTK_CONTAINER(table),3);

	button = gtk_radio_button_new_with_label(NULL,"Fahrenheit");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (button));
	gtk_table_attach (GTK_TABLE (table), button, 1, 2, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND),
			(GtkAttachOptions) (0), 0, 0);
	if (temp_units == FAHRENHEIT)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),TRUE);
	g_object_set_data(G_OBJECT(button),"handler",GINT_TO_POINTER(FAHRENHEIT));
	g_signal_connect(G_OBJECT(button),"toggled",
			G_CALLBACK(toggle_button_handler),
			NULL);


	button = gtk_radio_button_new_with_label(group,"Celsius");
	group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (button));
	gtk_table_attach (GTK_TABLE (table), button, 3, 4, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND),
			(GtkAttachOptions) (0), 0, 0);
	if (temp_units == CELSIUS)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button),TRUE);
	g_object_set_data(G_OBJECT(button),"handler",GINT_TO_POINTER(CELSIUS));
	g_signal_connect(G_OBJECT(button),"toggled",
			G_CALLBACK(toggle_button_handler),
			NULL);

	ebox = gtk_event_box_new();
	gtk_box_pack_start(GTK_BOX(vbox),ebox,FALSE,TRUE,0);
	gtk_tooltips_set_tip(tip,ebox,
			"This box provides your choice for the active dashboard to be used",NULL);

	frame = gtk_frame_new("Dashboard(s) Selection");
	gtk_container_add(GTK_CONTAINER(ebox),frame);
	vbox2 = gtk_vbox_new(FALSE,5);
	gtk_container_add(GTK_CONTAINER(frame),vbox2);
	hbox = gtk_hbox_new(TRUE,5);
	gtk_container_set_border_width(GTK_CONTAINER(hbox),5);
	gtk_box_pack_start(GTK_BOX(vbox2),hbox,FALSE,TRUE,0);

	hbox2 = gtk_hbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),hbox2,TRUE,TRUE,0);
	cbutton = gtk_check_button_new_with_label(NULL);
	g_signal_connect(G_OBJECT(cbutton),"toggled",
			G_CALLBACK(remove_dashboard),GINT_TO_POINTER(1));
	gtk_box_pack_start(GTK_BOX(hbox2),cbutton,FALSE,TRUE,0);

	button = gtk_button_new();
	if ((cluster_1_name) && (g_ascii_strcasecmp(cluster_1_name,"") !=0))
		label = gtk_label_new(cluster_1_name);
	else
		label = gtk_label_new("Choose a Dashboard File");
	g_object_set_data(G_OBJECT(button),"label",label);
	g_object_set_data(G_OBJECT(cbutton),"label",label);
#if GTK_MINOR_VERSION >= 6
	if (gtk_minor_version >= 6)
		gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_MIDDLE);
#endif
	gtk_container_add(GTK_CONTAINER(button),label);
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(present_dash_filechooser),
			GINT_TO_POINTER(1));
	gtk_box_pack_start(GTK_BOX(hbox2),button,TRUE,TRUE,0);
	register_widget("dash_cluster_1_label",label);

	hbox2 = gtk_hbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),hbox2,TRUE,TRUE,0);
	cbutton = gtk_check_button_new_with_label(NULL);
	g_signal_connect(G_OBJECT(cbutton),"toggled",
			G_CALLBACK(remove_dashboard),GINT_TO_POINTER(2));
	gtk_box_pack_start(GTK_BOX(hbox2),cbutton,FALSE,TRUE,0);

	button = gtk_button_new();
	if ((cluster_2_name) && (g_ascii_strcasecmp(cluster_2_name,"") !=0))
		label = gtk_label_new(cluster_2_name);
	else
		label = gtk_label_new("Choose a Dashboard File");
	g_object_set_data(G_OBJECT(button),"label",label);
	g_object_set_data(G_OBJECT(cbutton),"label",label);
#if GTK_MINOR_VERSION >= 6
	if (gtk_minor_version >= 6)
		gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_MIDDLE);
#endif
	gtk_container_add(GTK_CONTAINER(button),label);
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(present_dash_filechooser),
			GINT_TO_POINTER(2));
	gtk_box_pack_start(GTK_BOX(hbox2),button,TRUE,TRUE,0);
	register_widget("dash_cluster_2_label",label);

	ebox = gtk_event_box_new();
	gtk_box_pack_start(GTK_BOX(vbox),ebox,FALSE,TRUE,0);
	gtk_tooltips_set_tip(tip,ebox,
			"This box gives you the debugging choices.  Each one is independantly selectable.  Logging output will appear on the console that MegaTunix was started from...",NULL);

	frame = gtk_frame_new("MegaTunix Debugging");
	gtk_container_add(GTK_CONTAINER(ebox),frame);
	vbox2 = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame),vbox2);

	table = gtk_table_new(ceil(num_levels/4),4,FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table),5);
	gtk_table_set_col_spacings(GTK_TABLE(table),5);
	gtk_container_set_border_width(GTK_CONTAINER(table),5);
	gtk_box_pack_start(GTK_BOX(vbox2),table,FALSE,TRUE,5);

	j = 0;
	k = 0;
	for (i=0;i<num_levels;i++)
	{
		mask = dbglevels[i].dclass;
		shift = dbglevels[i].dshift;

		button = gtk_check_button_new_with_label(dbglevels[i].name);
		g_object_set_data(G_OBJECT(button),"handler",GINT_TO_POINTER(dbglevels[i].handler));
		g_object_set_data(G_OBJECT(button),"bitshift",GINT_TO_POINTER(shift));
		g_object_set_data(G_OBJECT(button),"bitmask",GINT_TO_POINTER(mask));
		g_object_set_data(G_OBJECT(button),"bitval",GINT_TO_POINTER(1));
		g_signal_connect(G_OBJECT(button),"toggled",
				G_CALLBACK(bitmask_button_handler),
				NULL);
		gtk_table_attach (GTK_TABLE (table), button, j, j+1, k, k+1,
				(GtkAttachOptions) (GTK_FILL),
				(GtkAttachOptions) (0), 0, 0);
		// If user set on turn on as well
		if ((dbg_lvl & mask) >> shift)
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(button),TRUE);
		// if hardcoded on, turn on..
		if (dbglevels[i].enabled)
			gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(button),TRUE);
		j++;
		if (j == 4)
		{
			k++;
			j = 0;
		}
	}

	ebox = gtk_event_box_new();
	gtk_box_pack_start(GTK_BOX(vbox),ebox,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip,ebox,
			"This box shows you the MegaSquirt Interrogation report.  Due to the rise of various MegaSquirt variants, several of them unfortunately return the same version number except that their API's aren't compatible.  This window give you some feedback about how the MS responds to various commands and suggests what it thinks is the closest match.",NULL);

	frame = gtk_frame_new("MegaSquirt ECU Information");
	gtk_container_add(GTK_CONTAINER(ebox),frame);
	vbox2 = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame),vbox2);

	table = gtk_table_new(5,2,FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table),3);
	gtk_table_set_col_spacings(GTK_TABLE(table),5);
	gtk_container_set_border_width(GTK_CONTAINER(table),5);
	gtk_box_pack_start(GTK_BOX(vbox2),table,TRUE,TRUE,5);

	hbox = gtk_hbox_new(TRUE,30);
	gtk_table_attach (GTK_TABLE (table), hbox, 0, 2, 0, 1,
			(GtkAttachOptions) (GTK_EXPAND|GTK_SHRINK|GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);

	ebox = gtk_event_box_new();
	gtk_box_pack_start(GTK_BOX(hbox),ebox,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip,ebox,
			"This button interrogates the connected ECU to attempt to determine what firmware is loaded and to setup the gui to adapt to the capabilities of the loaded version. This method is not 100\% foolproof, but it works about 99.5% of hte time.  If it MIS-detects your ECU contact the developer with your firmware details.",NULL);
	button = gtk_button_new_with_label("Interrogate ECU capabilities");
	register_widget("interrogate_button",button);
	gtk_container_add(GTK_CONTAINER(ebox),button);
	g_object_set_data(G_OBJECT(button),"handler",GINT_TO_POINTER(INTERROGATE_ECU));
	g_signal_connect(G_OBJECT (button), "clicked",
			G_CALLBACK (std_button_handler),
			NULL);

	ebox = gtk_event_box_new();
	gtk_box_pack_start(GTK_BOX(hbox),ebox,TRUE,TRUE,0);
	gtk_tooltips_set_tip(tip,ebox,
			"This button Enables \"Offline Mode\" so that you can load tabs specific to an ECU and set settings, modify maps without doing any Serial I/O. This will allow you to modify maps offline when not connected to the vehicle/ECU.",NULL);
	button = gtk_button_new_with_label("Use Offline Mode");
	register_widget("offline_button",button);
	gtk_container_add(GTK_CONTAINER(ebox),button);
	g_object_set_data(G_OBJECT(button),"handler",GINT_TO_POINTER(OFFLINE_MODE));
	g_signal_connect(G_OBJECT (button), "clicked",
			G_CALLBACK (std_button_handler),
			NULL);


	label = gtk_label_new("ECU Revision Number");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);

	hbox = gtk_hbox_new(FALSE,0);
	gtk_table_attach (GTK_TABLE (table), hbox, 1, 2, 1, 2,
			(GtkAttachOptions) (GTK_EXPAND|GTK_SHRINK|GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);

	entry = gtk_entry_new();
	register_widget("ecu_revision_entry",entry);
	gtk_entry_set_width_chars (GTK_ENTRY (entry), 5);
	gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
	gtk_box_pack_start(GTK_BOX(hbox),entry,FALSE,FALSE,0);

	label = gtk_label_new("Text Version");
	gtk_misc_set_alignment(GTK_MISC(label),0.0,0.5);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);

	entry = gtk_entry_new();
	register_widget("text_version_entry",entry);
	gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
	gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 2, 3,
			(GtkAttachOptions) (GTK_SHRINK|GTK_FILL|GTK_EXPAND),
			(GtkAttachOptions) (0), 0, 0);

	label = gtk_label_new("ECU Signature");
	gtk_misc_set_alignment(GTK_MISC(label),0.0,0.5);
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4,
			(GtkAttachOptions) (GTK_FILL),
			(GtkAttachOptions) (0), 0, 0);

	entry = gtk_entry_new();
	register_widget("ecu_signature_entry",entry);
	gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
	gtk_table_attach (GTK_TABLE (table), entry, 1, 2, 3, 4,
			(GtkAttachOptions) (GTK_SHRINK|GTK_FILL|GTK_EXPAND),
			(GtkAttachOptions) (0), 0, 0);

	ebox = gtk_event_box_new();
	gtk_tooltips_set_tip(tip,ebox,
			"This window shows the status of the ECU interrogation progress.  The way it works is that we send commands to the ECU and count how much data is returned, which helps us hone in to which firmware for the MS is in use.  This method is not 100\% foolproof, as some firmware editions return the same amount of data, AND the same version number making them indistinguishable from the outside interface.  The commands sent are:\n \"R\", which returns the extended runtime variables (only supported by a subset of firmwares, like MSnS-Extra \n \"A\" which returns the runtime variables (22 bytes usually)\n \"C\" which should return the MS clock (1 byte,  but this call fails on the (very old) version 1 MS's)\n \"Q\" Which should return the version number of the firmware multipled by 10\n \"V\" which should return the VEtable and constants, this size varies based on the firmware\n \"S\" which is a \"Signature Echo\" used in some of the variants.  Similar to the \"T\" command (Text version)\n \"I\" which returns the igntion table and related constants (ignition variants ONLY)\n The \"F0/1\" Commands return the raw memory of the MegaSquirt ECU (DT Firmwares only).",NULL);

	gtk_table_attach (GTK_TABLE (table), ebox, 0, 2, 4, 5,
			(GtkAttachOptions) (GTK_EXPAND|GTK_SHRINK|GTK_FILL),
			(GtkAttachOptions) (GTK_EXPAND|GTK_SHRINK|GTK_FILL), 0, 0);

	frame = gtk_frame_new ("Interrogation/Tab load Status");
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(ebox),frame);

	vbox2 = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(frame),vbox2);

	sw = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);
	//	gtk_widget_set_size_request(sw,-1,200);
	gtk_box_pack_start(GTK_BOX(vbox2),sw,TRUE,TRUE,5);

	view = gtk_text_view_new ();
	register_widget("interr_view",view);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(view),FALSE);
	textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	gtk_text_buffer_create_tag(textbuffer,
			"warning",
			"foreground",
			"red", NULL);
	gtk_text_buffer_create_tag(textbuffer,
			"info",
			"foreground",
			"dark green", NULL);
	gtk_container_add(GTK_CONTAINER(sw),view);

	return;
}
