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
#include <datalogging_const.h>
#include <defines.h>
#include <enums.h>
#include <gui_handlers.h>
#include <logviewer_gui.h>
#include <ms_structures.h>
#include <structures.h>

static gint max_viewables = 0;
static gint total_viewables = 0;
static struct Logables viewables;
static GHashTable *active_traces = NULL;
static gint red = 0;
static gint green = 32768;
static gint blue = 65535;

/* This table is the same dimensions as the table used for datalogging.
 * FALSE means it's greyed out as a choice for the logviewer, TRUE means
 * it's visible.  Some logable variables (like the clock) don't make a lot
 * of sense on a stripchart view...
 */
static const gboolean valid_logables[]=
{
	FALSE,TRUE,TRUE,FALSE,TRUE,
	TRUE,TRUE,TRUE,TRUE,TRUE,
	TRUE,TRUE,TRUE,TRUE,TRUE,
	TRUE,TRUE,TRUE,TRUE,TRUE,
	TRUE,TRUE,TRUE,TRUE,TRUE,
	TRUE,TRUE,TRUE,TRUE,TRUE,
	TRUE,TRUE,TRUE,TRUE,TRUE,
	TRUE,TRUE,TRUE,TRUE
};

void build_logviewer(GtkWidget *parent_frame)
{
	GtkWidget *vbox;
	GtkWidget *vbox2;
	GtkWidget *vbox3;
	GtkWidget *vbox4;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *frame;
	extern struct DynamicButtons buttons;
	GSList *group;

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox),5);
	gtk_container_add(GTK_CONTAINER(parent_frame),vbox);

	/* Traces frame */
	frame = gtk_frame_new("Log Viewer");
	gtk_box_pack_start(GTK_BOX(vbox),frame,TRUE,TRUE,0);

	/* Holds all the log traces... */
	vbox2 = gtk_vbox_new(TRUE,2);
	gtk_container_set_border_width(GTK_CONTAINER(vbox2),5);
	gtk_container_add(GTK_CONTAINER(frame),vbox2);

	/* Settings/Parameters frame... */
	frame = gtk_frame_new("Playback/Viewer Parameters");
	gtk_box_pack_start(GTK_BOX(vbox),frame,FALSE,FALSE,0);

	hbox = gtk_hbox_new(FALSE,15);
	gtk_container_add(GTK_CONTAINER(frame),hbox);

	/* Hold the realtime/[playback buttons */
	vbox3 = gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox),vbox3,FALSE,FALSE,0);

	button = gtk_radio_button_new_with_label(NULL,"Realtime Mode");
	gtk_box_pack_start(GTK_BOX(vbox3),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button), "toggled",
			G_CALLBACK(toggle_button_handler),
			GINT_TO_POINTER(REALTIME_VIEW));

	group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(button));
	button = gtk_radio_button_new_with_label(group,"Playback Mode");
	gtk_box_pack_start(GTK_BOX(vbox3),button,FALSE,FALSE,0);
	g_signal_connect(G_OBJECT(button), "toggled",
			G_CALLBACK(toggle_button_handler),
			GINT_TO_POINTER(PLAYBACK_VIEW));

	/* Holds the Select Button */
	vbox4 = gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox),vbox4,FALSE,FALSE,0);

	button = gtk_button_new_with_label("Select Logfile to Playback");
	buttons.logplay_sel_log_but = button;
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(std_button_handler),
			GINT_TO_POINTER(SELECT_DLOG_IMP));
	gtk_box_pack_start(GTK_BOX(vbox4),button,TRUE,FALSE,0);
	gtk_widget_set_sensitive(button,FALSE);
	
	button = gtk_button_new_with_label("Select Parameters to view");
	buttons.logplay_sel_parm_but = button;
	g_object_set_data(G_OBJECT(button),"data",(gpointer)vbox2);
	g_signal_connect(G_OBJECT(button),"clicked",
			G_CALLBACK(std_button_handler),
			GINT_TO_POINTER(SELECT_PARAMS));
	gtk_box_pack_start(GTK_BOX(vbox4),button,TRUE,FALSE,0);
	
	vbox4 = gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(hbox),vbox4,FALSE,FALSE,0);
	button = gtk_button_new_with_label("Start Reading RT Vars");
	buttons.logplay_start_rt_but = button;
        g_signal_connect(G_OBJECT (button), "clicked",
                        G_CALLBACK (std_button_handler), \
                        GINT_TO_POINTER(START_REALTIME));
	gtk_box_pack_start(GTK_BOX(vbox4),button,TRUE,FALSE,0);
	button = gtk_button_new_with_label("Stop Reading RT vars");
	buttons.logplay_stop_rt_but = button;
        g_signal_connect(G_OBJECT (button), "clicked",
                        G_CALLBACK (std_button_handler), \
                        GINT_TO_POINTER(STOP_REALTIME));

	gtk_box_pack_start(GTK_BOX(vbox4),button,TRUE,FALSE,0);


	
	/* Not written yet */
	return;
}

void present_viewer_choices( void *ptr)
{
	GtkWidget *window;
	GtkWidget *table;
	GtkWidget *frame;
	GtkWidget *vbox;
	GtkWidget *button;
	GtkWidget *sep;
	extern GtkTooltips *tip;
	gint i = 0;
	gint j = 0;
	gint k = 0;
	gint table_rows;
	gint table_cols = 5;
	GtkWidget *special = NULL;

	if (ptr != NULL)
		special = (GtkWidget *)ptr;
	else
		printf("pointer fed was NULL (present_viewer_choices)\n");

	/* basty hack to prevent a compiler warning... */
	max_viewables = sizeof(mt_compat_names)/sizeof(gchar *);
	max_viewables = sizeof(logable_names)/sizeof(gchar *);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window),575,300);
	gtk_window_set_title(GTK_WINDOW(window),"Logviewer Realtime choices");
	g_signal_connect_swapped(G_OBJECT(window),"destroy_event",
			G_CALLBACK(gtk_widget_destroy),
			(gpointer)window);
	g_signal_connect_swapped(G_OBJECT(window),"delete_event",
			G_CALLBACK(gtk_widget_destroy),
			(gpointer)window);

	gtk_container_set_border_width(GTK_CONTAINER(window),5);
	frame = gtk_frame_new("Select Variables to view from the list below");
	gtk_container_add(GTK_CONTAINER(window),frame);

	vbox = gtk_vbox_new(FALSE,0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
	gtk_container_add(GTK_CONTAINER(frame),vbox);

	max_viewables = sizeof(logable_names)/sizeof(gchar *);
	table_rows = ceil((float)max_viewables/(float)table_cols);
	table = gtk_table_new(table_rows,table_cols,TRUE);
	gtk_table_set_row_spacings(GTK_TABLE(table),5);
	gtk_table_set_col_spacings(GTK_TABLE(table),10);
	gtk_container_set_border_width(GTK_CONTAINER(table),0);
	gtk_box_pack_start(GTK_BOX(vbox),table,FALSE,FALSE,0);

	j = 0;
	k = 0;
	for (i=0;i<max_viewables;i++)
	{
		button = gtk_check_button_new_with_label(logable_names[i]);
		if (valid_logables[i] == FALSE)
			gtk_widget_set_sensitive(button,FALSE);
		gtk_tooltips_set_tip(tip,button,logable_names_tips[i],NULL);
		if (viewables.index[i] == TRUE)
        		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button),TRUE);
			
		viewables.widgets[i] = button;

		g_object_set_data(G_OBJECT(button),"index",
				GINT_TO_POINTER(i));
		g_signal_connect(G_OBJECT(button),"toggled",
				G_CALLBACK(view_value_set),
				NULL);
		gtk_table_attach (GTK_TABLE (table), button, j, j+1, k, k+1,
				(GtkAttachOptions) (GTK_FILL),
				(GtkAttachOptions) (0), 0, 0);
		j++;

		if (j == 5)
		{
			k++;
			j = 0;
		}
	}

	sep = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox),sep,TRUE,TRUE,0);
	button = gtk_button_new_with_label("Close");
	gtk_box_pack_start(GTK_BOX(vbox),button,FALSE,TRUE,0);
	g_signal_connect_swapped(G_OBJECT(button),"clicked",
			G_CALLBACK(populate_viewer),
			(gpointer)special);
	g_signal_connect_swapped(G_OBJECT(button),"clicked",
			G_CALLBACK(gtk_widget_destroy),
			(gpointer)window);

	gtk_widget_show_all(window);
	return;
}

gboolean view_value_set(GtkWidget *widget, gpointer data)
{
	gint index = 0;
        gint i = 0;

        index = (gint)g_object_get_data(G_OBJECT(widget),"index");

        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
                viewables.index[index] = TRUE;
        else
                viewables.index[index] = FALSE;

        total_viewables = 0;
        for (i=0;i<max_viewables;i++)
        {
                if (viewables.index[i])
                        total_viewables++;
        }

	//printf("total viewables is %i\n",total_viewables);

	return TRUE;
}

gboolean populate_viewer(GtkWidget * widget)
{
	struct Viewable_Value *v_value = NULL;
	gint i = 0;

	/* Checks if list is created, if not,  makes one, allocates data
	 * for strcutres defining each viewable element., sets those attribute
	 * and adds the mto the list,  also checks if entires are removed and
	 * pulles them fromthe list and de-allocates them...
	 */
	if (active_traces == NULL)
	{
		active_traces = g_hash_table_new(NULL,NULL);
	}
	
	/* check to see if it's already in the table, if so ignore, if not
	 * malloca datastructure, populate it's values and insert a pointer
	 * into the table for it..
	 */
	for (i=0;i<max_viewables;i++)
	{
		/* if not found in table check to see if we need to insert*/
		if (g_hash_table_lookup(active_traces,GINT_TO_POINTER(i))==NULL)
		{
			if (viewables.index[i])	/* Marked viewable widget */
			{
				//printf("allocating struct and putting into table\n");
				/* Call the build routine, feed it it's parent*/
				v_value = build_v_value(widget,i);
					
//				printf("put in offset %i, runtime_offset %i, size %i\n",i,v_value->runtime_offset, v_value->size);
				g_hash_table_insert(active_traces,
						GINT_TO_POINTER(i),
						(gpointer)v_value);
			}
		}
		else
		{	/* If in table but now de-selected, remove it */
			if (!viewables.index[i])
			{
				v_value = (struct Viewable_Value *)
					g_hash_table_lookup(
							active_traces,
							GINT_TO_POINTER(i));
				/* Remove entry in from hash table */
				g_hash_table_remove(active_traces,
						GINT_TO_POINTER(i));

				/* Free all resources of the datastructure 
				 * before de-allocating it... 
				 */
				gtk_widget_destroy(v_value->d_area);
				g_array_free(v_value->data_array,TRUE);
				g_object_unref(v_value->trace_pmap);
				g_object_unref(v_value->trace_gc);
				g_object_unref(v_value->font_gc);
				g_object_unref(v_value->grat_gc);
				g_free(v_value->vname);
				g_free(v_value);
				v_value = NULL;
			}
		}
	}
		
	return FALSE; /* want other handlers to run... */
}

gboolean lv_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	GdkPixmap *trace_pmap = NULL;
	gint w = 0;
	gint h = 0;
	struct Viewable_Value *v_value = NULL;

	/* Get rest of important data... */
	v_value = (struct Viewable_Value *)g_object_get_data(G_OBJECT(widget),
			"data");
	trace_pmap = v_value->trace_pmap;

	/* Trace is the full size of the drawing area AND the backing 
	 * pixmap forthe window...
	 */
	if (trace_pmap)
		g_object_unref(trace_pmap);

	w=event->width;
	h=event->height;
	trace_pmap=gdk_pixmap_new(widget->window,
			w,h,
			gtk_widget_get_visual(widget)->depth);
	gdk_draw_rectangle(trace_pmap,
			widget->style->black_gc,
			TRUE, 0,0,
			w,h);
	gdk_window_set_back_pixmap(widget->window,trace_pmap,0);

	v_value->trace_pmap = trace_pmap;

	draw_graticule(v_value);
	draw_infotext(v_value);
	draw_old_data(v_value);

	gdk_window_clear(widget->window);
//	printf("configure event....\n");
	return TRUE;
}

gboolean lv_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	GdkPixmap *trace_pmap = NULL;
	struct Viewable_Value *v_value = NULL;
	v_value = (struct Viewable_Value *)g_object_get_data(G_OBJECT(widget),
				"data");

	/* Expose event handler... */
	trace_pmap = v_value->trace_pmap;
	gdk_draw_drawable(widget->window,
                        widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                        trace_pmap,
                        event->area.x, event->area.y,
                        event->area.x, event->area.y,
                        event->area.width, event->area.height);

//	printf("expose event....\n");
	return TRUE;
}

struct Viewable_Value * build_v_value(GtkWidget * parent_widget, gint offset)
{
	struct Viewable_Value *v_value = NULL;
	v_value = g_malloc(sizeof(struct Viewable_Value));		

	/* Set limits of this variable. (it's ranges, used for scaling */
	v_value->lower = limits[offset].lower;
	v_value->upper = limits[offset].upper;
	/* Sets last "y" value to -1, needed for initial draw to be correct */
	v_value->last_y = -1;

	/* Tell where we get the data from */
	v_value->runtime_offset = logging_offset_map[offset];
	/* How big the data is (needed when indexing into the datablock */
	v_value->size = logging_datasizes_map[offset];
	/* textual name of the variabel we're viewing.. */
	v_value->vname = g_strdup(logable_names[offset]);

	/* Array to keep history for resize/redraw and export to datalog
	 * we use the _sized_ version to give a big enough size to prevent
	 * reallocating memory too often. (more initial mem usage,  but less
	 * calls to malloc...
	 */
	v_value->data_array = g_array_sized_new(FALSE,TRUE,sizeof(gfloat),4096);

	/* Set parent widget... */
	v_value->parent = parent_widget;
	/* create new drawing area... */
	v_value->d_area = gtk_drawing_area_new();

	/* Pixmap for the drawingarea for this logable value... */
	v_value->trace_pmap = gdk_pixmap_new(
			v_value->d_area->window, 
			10,10, 
			gtk_widget_get_visual(v_value->d_area)->depth);

	/* space set aside for textual information... */
	v_value->info_width = 100;

	/* place it */
	gtk_box_pack_start(GTK_BOX(parent_widget),
			v_value->d_area,
			TRUE,TRUE,0);

	/* Add events to capture mouse button presses (for popup menus...) */
	gtk_widget_add_events(v_value->d_area,
			GDK_BUTTON_PRESS_MASK |
			GDK_BUTTON_RELEASE_MASK |
			GDK_FOCUS_CHANGE_MASK);

	g_signal_connect(G_OBJECT
			(v_value->d_area),
			"configure_event",
			G_CALLBACK(lv_configure_event),
			NULL);

	g_signal_connect(G_OBJECT
			(v_value->d_area),
			"expose_event",
			G_CALLBACK(lv_expose_event),
			NULL);

	/* Bind a pointer to the datastructure that contains all the info 
	 * on this viewed trace, so the signal handlers can do their job
	 * without needing a million ugly global variables... */
	g_object_set_data(G_OBJECT(v_value->d_area),
			"data",(gpointer)v_value);

	/* Allocate the colors (GC's) for the font, trace and graticule */
	v_value->font_gc = initialize_gc(v_value->trace_pmap, FONT);
	v_value->trace_gc = initialize_gc(v_value->trace_pmap, TRACE);
	v_value->grat_gc = initialize_gc(v_value->trace_pmap, GRATICULE);

	gtk_widget_show_all(parent_widget);
	return v_value;
}


GdkGC * initialize_gc(GdkDrawable *drawable, GcType type)
{
	GdkColor color;
	GdkGC * gc = NULL;
	GdkGCValues values;
	GdkColormap *cmap = NULL;

	cmap = gdk_colormap_get_system();

	switch((GcType)type)
	{
		case FONT:
			color.red = 10000;
			color.green = 65535;
			color.blue = 10000;
			gdk_colormap_alloc_color(cmap,&color,TRUE,TRUE);
			values.foreground = color;
			gc = gdk_gc_new_with_values(GDK_DRAWABLE(drawable),
					&values,
					GDK_GC_FOREGROUND);
			break;

		case TRACE:
			color.red = red;
			color.green = green;
			color.blue = blue;
			gdk_colormap_alloc_color(cmap,&color,TRUE,TRUE);
			values.foreground = color;
			gc = gdk_gc_new_with_values(GDK_DRAWABLE(drawable),
					&values,
					GDK_GC_FOREGROUND);
			red += 24000;
			green += 24000;
			blue += 24000;
			if (red > 65536)
				red -= 65536;
			if (green > 65536)
				green -= 65536;
			if (blue > 65536)
				blue -= 65536;
			break;
		case GRATICULE:
			color.red = 36288;
			color.green = 2048;
			color.blue = 2048;
			gdk_colormap_alloc_color(cmap,&color,TRUE,TRUE);
			values.foreground = color;
			gc = gdk_gc_new_with_values(GDK_DRAWABLE(drawable),
					&values,
					GDK_GC_FOREGROUND);
			break;
	}	
	return gc;	
}

void draw_infotext(void *data)
{
	/* Draws the textual (static) information on the left side of 
	 * the window..
	 */
	struct Viewable_Value *v_value = (struct Viewable_Value *)data;
        PangoFontDescription *font_desc;
	PangoLayout *layout;

	font_desc = pango_font_description_from_string("sans 10");
	layout = gtk_widget_create_pango_layout(v_value->d_area,v_value->vname);
	pango_layout_set_font_description(layout,font_desc);
	gdk_draw_layout(v_value->trace_pmap,v_value->font_gc,0,0,layout);

}

void draw_old_data(void *data)
{
	gint lo_width = 0;
	gint w = 0;
	gint h = 0;
	gint total = 0;
	gint len = 0;
	gint i = 0;
	gfloat val = 0.0;
	gfloat percent = 0.0;
	GdkPoint pts[2048];	/*bad idea as static... */
	struct Viewable_Value *v_value = (struct Viewable_Value *)data;

	w = v_value->d_area->allocation.width;
	h = v_value->d_area->allocation.height;

	lo_width = v_value->d_area->allocation.width-v_value->info_width;
	total = v_value->data_array->len < lo_width  
		? v_value->data_array->len : lo_width;

	len = v_value->data_array->len;
	/* Draw is reverse order, from right to left, easier to think out
	 * in my head... :) 
	 */
	for (i=0;i<total;i++)
	{
		val = g_array_index(v_value->data_array,gfloat,len-i);
		percent = 1.0-(val/(v_value->upper-v_value->lower));
		pts[i].x = w-i;
		pts[i].y = (gint) (percent*h);
	}
	gdk_draw_lines(v_value->trace_pmap,
			v_value->trace_gc,
			pts,
			total);

	return;
}

void draw_graticule(void * data)
{
	/* Draws graticule for the v_value passed to it.. */
	gint lo_width = 0;
	gint lo_height = 0;
	gint i = 0;
	gint count = 0;
	gint grat_interval = 40;
	//gint rem = 0;
	GdkSegment segs[200];	/*bad idea to do statically*/
	struct Viewable_Value *v_value = (struct Viewable_Value *)data;

	lo_width = v_value->d_area->allocation.width-v_value->info_width;
	lo_height = v_value->d_area->allocation.height;
	v_value->grat_interval = 40;
		
	for (i=0;i<lo_width;i+=grat_interval)
	{

		segs[count].x1 = v_value->info_width+i;
		segs[count].x2 = v_value->info_width+i;
		segs[count].y1 = 0;
		segs[count].y2 = lo_height;
		count++;
	}
	gdk_draw_segments(v_value->trace_pmap,v_value->grat_gc,segs,count);
	v_value->last_grat = segs[count-1].x1;
/*	rem = lo_height%40;
	count = 0;
	for (i=0;i<lo_height;i+=40)
	{
		segs[count].x1 = v_value->info_width;
		segs[count].x2 = v_value->info_width+lo_width;
		segs[count].y1 = rem/2+i;
		segs[count].y2 = rem/2+i;
		
		count++;
	}
	gdk_draw_segments(v_value->trace_pmap,v_value->grat_gc,segs,count);
*/
		
}

gboolean update_logview_traces()
{
	/* Called from one of two possible places:
	 * 1. a GTK timeout used when playing back a log...
	 * 2. as a hook onto the update_realtime stats box...
	 */
	
	if (active_traces)
		g_hash_table_foreach(active_traces, trace_update,NULL);
	return TRUE;
}

void trace_update(gpointer key, gpointer value, gpointer data)
{
	gint w = 0;
	gint h = 0;
	gint start = 0;
	gint end = 0;
	gfloat val = 0.0;
	gfloat percent = 0.0;
	gint size = 0;
	gint scroll = 1;
	gint last_grat = 0;
	gint grat_interval = 0;
	struct Viewable_Value * v_value = NULL;
	extern struct Runtime_Common *runtime;
	unsigned char * uc_ptr = NULL;
	short * s_ptr = NULL;
	float * f_ptr = NULL;
	

	v_value = (struct Viewable_Value *) value;
	uc_ptr = (unsigned char *)runtime;
	s_ptr = (short *)runtime;
	f_ptr = (float *)runtime;

	size = v_value->size;
	/* this weird way is needed cause we are accessing various sized
	 * objects from a datastructure in array notation.  This works as 
	 * long as the structure is arranged in largest order first. 
	 * i.e. floats before shorts before chars, etc...
	 */
	switch (size)
	{	
		case FLOAT:
			val = (float)f_ptr[v_value->runtime_offset/FLOAT];
			break;
		case UCHAR:
			val = (unsigned char)uc_ptr[v_value->runtime_offset];
			break;
		case SHORT:
			val = (short)s_ptr[v_value->runtime_offset/SHORT];
			break;
	}
	if (val > (v_value->max))
		v_value->max = val;
	if (val < (v_value->min))
		v_value->min = val;

	g_array_append_val(v_value->data_array,val);

	last_grat = v_value->last_grat;
	grat_interval = v_value->grat_interval;
	
	end = v_value->info_width;
	start = end + scroll;
	w = v_value->d_area->allocation.width;
	h = v_value->d_area->allocation.height;

	/* Scroll the screen to the left... */
	gdk_draw_drawable(v_value->trace_pmap,
			v_value->trace_gc,
			v_value->trace_pmap,
			start,0,
			end,0,
			w-start,h);

	gdk_draw_rectangle(v_value->trace_pmap,
			v_value->d_area->style->black_gc,
			TRUE,
			w-scroll,0,
			scroll,h);

	/* Update the graticule... */
	v_value->last_grat -= scroll;
	if ((last_grat + grat_interval) < w)
	{
		gdk_draw_line(v_value->trace_pmap,
				v_value->grat_gc,
				w-1, 0,
				w-1, h);

		v_value->last_grat = w-1;
	}

	/* Draw thedata.... */
	percent = 1.0-(val/(v_value->upper-v_value->lower));
	if (v_value->last_y == -1)
		v_value->last_y = (gint)(percent*h);
	
	gdk_draw_line(v_value->trace_pmap,
			v_value->trace_gc,
			w-1-scroll,v_value->last_y,
			w-1,(gint)(percent*h));

	v_value->last_y = (gint)(percent*h);
			
	gdk_window_clear(v_value->d_area->window);
}
