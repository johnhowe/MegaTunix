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


#ifndef __PROTOS_H__
#define __PROTOS_H__

#include <config.h>
#include "globals.h"
#include "defines.h"
#include "configfile.h"
#include <gtk/gtk.h>

/* Function Prototypes for all objects/source files */

/* for each of the *_gui.c files will be  a main core function called 
 * int build_****(Gtkwidget *).  The "****" will be that core function 
 * wheter it be "tools", "datalogging", "constants", or whatever.  The 
 * arg is a "Gtkwidget *" which is the PARENT widget to which all gui
 * sub-ojects must be placed into (multi-level embedding is encouraged 
 * to improve the viewability of the data.
 */

/* about_gui.c */
int build_about(GtkWidget *);
/* abou_guit.c */

/* comms_gui.c */
int build_comms(GtkWidget *);
void update_errcounts(void);
/* comms_gui.c */

/* configfile.c function protos, derived from XMMS */
ConfigFile *cfg_new(void);
ConfigFile *cfg_open_file(gchar * filename);
gboolean cfg_write_file(ConfigFile * cfg, gchar * filename);
void cfg_free(ConfigFile * cfg);
gboolean cfg_read_string(ConfigFile * cfg, gchar * section, \
		gchar * key, gchar ** value);
gboolean cfg_read_int(ConfigFile * cfg, gchar * section, \
		gchar * key, gint * value);
gboolean cfg_read_boolean(ConfigFile * cfg, gchar * section, \
		gchar * key, gboolean * value);
gboolean cfg_read_float(ConfigFile * cfg, gchar * section, \
		gchar * key, gfloat * value);
gboolean cfg_read_double(ConfigFile * cfg, gchar * section, \
		gchar * key, gdouble * value);
void cfg_write_string(ConfigFile * cfg, gchar * section, \
		gchar * key, gchar * value);
void cfg_write_int(ConfigFile * cfg, gchar * section, \
		gchar * key, gint value);
void cfg_write_boolean(ConfigFile * cfg, gchar * section, \
		gchar * key, gboolean value);
void cfg_write_float(ConfigFile * cfg, gchar * section, \
		gchar * key, gfloat value);
void cfg_write_double(ConfigFile * cfg, gchar * section, \
		gchar * key, gdouble value);
void cfg_remove_key(ConfigFile * cfg, gchar * section, gchar * key);
/* Configfile.c function protos, derived from XMMS */

/* constants_gui.c */
int build_constants(GtkWidget *);
/* constants_gui.c */

/* conversions.c */
gboolean read_conversions(char *);
gint convert_before_download(gint, gfloat);
gfloat convert_after_upload(gint, gfloat);
/* conversions.c */

/* core_gui.c */
int setup_gui(void);
int framebuild_dispatch(GtkWidget *, int);
/* core_gui.c */

/* datalogging_gui.c */
int build_datalogging(GtkWidget *);
void create_dlog_filesel(void);
void check_filename(GtkWidget *, GtkFileSelection *);
void truncate_log(void);
void start_datalogging(void);
void stop_datalogging(void);
void close_logfile(void);
void clear_logables(void);
int set_logging_mode(GtkWidget *, gpointer);
int set_logging_delimiter(GtkWidget *, gpointer);
int log_value_set(GtkWidget *, gpointer);
void write_log_header(void);
void run_datalog(void);
/* datalogging_gui.c */

/* enrichments_gui.c */
int build_enrichments(GtkWidget *);
/* enrichments_gui.c */

/* general_gui.c */
int build_general(GtkWidget *);
/* general_gui.c */

/* gui_handlers.c */
void leave(GtkWidget *, gpointer);
int std_button_handler(GtkWidget *, gpointer);
int toggle_button_handler(GtkWidget *, gpointer);
int bitmask_button_handler(GtkWidget *, gpointer);
int spinner_changed(GtkWidget *, gpointer);
int classed_spinner_changed(GtkWidget *, gpointer);
void check_req_fuel_limits(void);
void check_config11(gint);
void check_config13(gint);
/* gui_handlers.c */

/* init.c */
void init(void);
int read_config(void);
void save_config(void);
void make_megasquirt_dirs(void);
void mem_alloc(void);
void mem_dealloc(void);
/* init.c */

/* lowlevel_gui.c */
int build_lowlevel(GtkWidget *);
/* lowlevel_gui.c */

/* main.c */
gboolean file_exists(const char *);
/* main.c */

/* ms_constants.c */
void post_process(struct raw_runtime_std *, struct runtime_std * );
/* ms_constants.c */

/* notifications.c */
void set_store_red(void);
void set_store_black(void);
void update_statusbar(GtkWidget *, int, gchar *);
void no_ms_connection(void);
void no_conversions_warning(void);
void warn_user(gchar *);
void squirt_cyl_inj_red(void);
void squirt_cyl_inj_black(void);
void warn_datalog_not_empty(void);
gint close_dialog(GtkWidget *, gpointer);
/* notifications.c */

/* req_fuel.c */
int reqd_fuel_popup();
int update_reqd_fuel(GtkWidget *, gpointer);
int close_popup(GtkWidget *, gpointer);
/* req_fuel.c */

/* runtime_gui.c */
int build_runtime(GtkWidget *);
void update_runtime_vars(void);
void reset_runtime_status(void);
/* runtime_gui.c */

/* serialio.c */
int open_serial(int); 
int setup_serial_params(void); 
void close_serial(void); 
int handle_ms_data(int); 
int check_ecu_comms(GtkWidget *, gpointer);
void read_ve_const(void);
void update_ve_const(void);
void write_ve_const(gint, gint);
void burn_flash(void);
/* serialio.c */

/* threads.c */
void * serial_raw_thread_starter(void *); /* bootstrap function to get IO started */
void * serial_raw_thread_stopper(void *); /* Realtime thread stopper */
void * raw_reader_thread(void *); /* Serial raw reader thread */
int stop_serial_thread();	/* cancels reader thread */
void start_serial_thread(void);	/*bootstrp function fopr above */
/* threads.c */

/* tools_gui.c */
int build_tools(GtkWidget *);
/* tools_gui.c */

/* tuning_gui.c */
int build_tuning(GtkWidget *);
/* tuning_gui.c */

/* vetable_gui.c */
int build_vetable(GtkWidget *);
/* vetable_gui.c */

#endif
