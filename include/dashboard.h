/*
 * Copyright (C) 2002-2012 by Dave J. Andruczyk <djandruczyk at yahoo dot com>
 *
 * Linux Megasquirt tuning software
 * 
 * 
 * This software comes under the GPL (GNU Public License)
 * You may freely copy,distribute, etc. this as long as all the source code
 * is made available for FREE.
 * 
 * No warranty is made or implied. You use this program at your own risk.
 */

/*!
  \file include/dashboard.h
  \ingroup Headers
  \brief Hander for dashboard management
  \author David Andruczyk
  */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __DASHBOARD_H__
#define __DASHBOARD_H__

#include <defines.h>
#include <gauge.h>
#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <libxml/tree.h>


typedef struct _Dash_Gauge Dash_Gauge;
/*!
 \brief The _Dash_Gauge struct contains info on the dashboard guages for 
 megatunix's two potential dashboards.
 */
struct _Dash_Gauge
{
	GData *object;			/*!< Data storage object for RT vars */
	gchar * source;			/*!< Data Source name */
	GtkWidget *gauge;		/*!< pointer to gauge itself */
	GtkWidget *dash;		/*!< pointer to gauge parent */
};

/* Prototypes */
GtkWidget * load_dashboard(const gchar *, gint);
gboolean remove_dashboard(GtkWidget *, gpointer );
void load_elements(GtkWidget *, xmlNode * );
void load_geometry(GtkWidget *, xmlNode *);
void load_gauge(GtkWidget *, xmlNode *);
void update_dash_gauge(gpointer , gpointer , gpointer );
void link_dash_datasources(GtkWidget *,gpointer);
void dash_shape_combine(GtkWidget *, gboolean);
gboolean dash_motion_event(GtkWidget *, GdkEventMotion *, gpointer );
gboolean dash_button_event(GtkWidget *, GdkEventButton *, gpointer );
gboolean dash_key_event(GtkWidget *, GdkEventKey *, gpointer );
void initialize_dashboards_pf(void);
gboolean present_dash_filechooser(GtkWidget *, gpointer );
gboolean remove_dashcluster(gpointer, gpointer , gpointer );
gboolean dummy(GtkWidget *,gpointer );
 void create_gauge(GtkWidget *);
gboolean hide_dash_resizers_wrapper(gpointer );
gboolean hide_dash_resizers(gpointer );
void update_tab_gauges(void);
gboolean dash_configure_event(GtkWidget * , GdkEventConfigure * );
gboolean enter_leave_event(GtkWidget * , GdkEventCrossing *, gpointer);
void dash_toggle_attribute(GtkWidget *, MtxGenAttr);
gboolean dash_popup_menu_handler(GtkWidget *);
void dash_context_popup(GtkWidget *, GdkEventButton *);
gboolean dash_lookup_attribute(GtkWidget *, MtxGenAttr);
gboolean toggle_dash_tattletales(GtkWidget *, gpointer);
gboolean toggle_dash_antialias(GtkWidget *, gpointer);
void toggle_dash_fullscreen(GtkWidget *, gpointer);
void toggle_dash_on_top(GtkWidget *, gpointer);
gboolean reset_dash_tattletales(GtkWidget *, gpointer);
gboolean toggle_dash_daytime(GtkWidget *, gpointer);
gboolean set_dash_time_mode(GtkWidget *, gpointer);
gboolean get_dash_daytime_mode(GtkWidget *);
gboolean ebox_expose(GtkWidget *, GdkEventExpose *, gpointer);
void set_dash_daytime_mode(GtkWidget *, gboolean);
gboolean close_dash(GtkWidget *, gpointer );
void toggle_main_visible(void);
void toggle_status_visible(void);
void toggle_rtt_visible(void);
void toggle_gui_visible(GtkWidget *widget, gpointer data);
void print_dash_choices(gchar *);
gboolean update_dashboards(gpointer);
gchar * validate_dash_choice(gchar *, gboolean *);
void dash_file_chosen(GtkFileChooserButton *, gpointer);
void dash_set_chooser_button_defaults(GtkFileChooser *);
/* Prototypes */

#endif
#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif
