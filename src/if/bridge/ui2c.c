/*
 * $Id: ui_core_interface.c,v 1.0 
 *	
 * Copyright (c) 2004, Emile Roberts
 *	
 * Interface UI -> core
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */


/*
 *	At this point the interface basically redirects function calls from the ui 
 *	to the core and vice-versa.  We may wish to split this file into multiple
 *	files at some point.  
 */

/*	
 *	SECTION 1 - Includes needed by the interface to allow ui/core communication
 */

#define CORE_SOURCES

/* includes ui needs to access core */
#include "lib/adns.h"
#include "if/core/bitzi.h"
#include "if/core/downloads.h"
#include "if/core/fileinfo.h"
#include "if/core/net_stats.h"
#include "if/core/hcache.h"
#include "if/core/hsep.h"
#include "if/core/http.h"
#include "if/core/parq.h"
#include "if/core/search.h"
#include "if/core/share.h"
#include "if/core/settings.h"
#include "if/core/uploads.h"
#include "if/core/version.h"
#include "if/core/main.h"

/*	
 *	Functions the UI uses to access the CORE
 */

/*	adns interface functions (UI -> Core)*/
gboolean guc_adns_resolve(
	const gchar *hostname, adns_callback_t user_callback, gpointer user_data)
{
	return adns_resolve(hostname, user_callback, user_data);
}

/*	bitzi interface functions (UI -> Core)*/
gboolean guc_bitzi_metadata_query(gpointer ptr)
{
	return bitzi_metadata_query(ptr);
}

void guc_bitzi_queue_metadata_search(guchar *sha1)
{
	bitzi_queue_metadata_search(sha1);
}

gboolean guc_bitzi_has_pending(void)
{
	return bitzi_has_pending();
}

/*	download and src interface functions (UI -> Core)*/
const gchar *guc_build_url_from_download(struct download *d) 
{
	return build_url_from_download(d);
}

gint guc_download_get_http_req_percent(const struct download *d)
{
	return download_get_http_req_percent(d);
}

void guc_download_fallback_to_push
	(struct download *d, gboolean on_timeout, gboolean user_request)
{ 
	download_fallback_to_push(d, on_timeout, user_request);
}

gint guc_download_remove_all_from_peer(gchar *guid, guint32 ip, 
	guint16 port, gboolean unavailable)
{
	return download_remove_all_from_peer(guid, ip, port, unavailable);
}

gint guc_download_remove_all_named(const gchar *name)
{
	return download_remove_all_named(name);
}

gint guc_download_remove_all_with_sha1(const gchar *sha1)
{
	return download_remove_all_with_sha1(sha1);
}

void guc_download_remove_file 
	(struct download *d, gboolean reset)
{
	download_remove_file(d, reset);
}

gboolean guc_download_file_exists(struct download *d)
{
	return download_file_exists(d);
}

void guc_download_requeue(struct download *d)
{
	download_requeue(d);
}

void guc_download_start
	(struct download *d, gboolean check_allowed)
{
	download_start(d, check_allowed); 
}

gboolean guc_download_remove(struct download *d)
{
	return download_remove(d);
}

void guc_download_abort(struct download *d)
{
	download_abort(d);
}

void guc_download_resume(struct download *d)
{
	download_resume(d);
}

void guc_download_freeze_queue(void)
{
	download_freeze_queue();
}

void guc_download_thaw_queue(void)
{
	download_thaw_queue();
}

gint guc_download_queue_is_frozen(void)
{
	return download_queue_is_frozen();
}

void guc_download_clear_stopped(gboolean complete,
	gboolean failed, gboolean unavailable, gboolean now)
{
	download_clear_stopped(complete, failed, unavailable, now);
}

void guc_download_auto_new(gchar *file, guint32 size, 
	guint32 record_index, guint32 ip, guint16 port, gchar *guid, 
	gchar *hostname, gchar *sha1, time_t stamp, gboolean push,
	gboolean file_size_known, struct dl_file_info *fi, 
	gnet_host_vec_t *proxies)
{
	download_auto_new(file, size, record_index, ip, port, guid, hostname, sha1,
		stamp, push, file_size_known, fi, proxies);
}

gboolean guc_download_new_unknown_size(gchar *file, 
			guint32 record_index, guint32 ip, guint16 port, gchar *guid, 
			gchar *hostname, gchar *sha1, time_t stamp, gboolean push,
			struct dl_file_info *fi, gnet_host_vec_t *proxies)
{
	return download_new_unknown_size(file, record_index, ip, port, guid, 
		hostname, sha1, stamp, push, fi, proxies);
}

const gchar *guc_download_get_hostname(const struct download *d)
{
	return download_get_hostname(d);
}

gfloat guc_download_source_progress(struct download *d)
{
	return download_source_progress(d);
}

gfloat guc_download_total_progress(struct download *d)
{
	return download_total_progress(d);	
}

gboolean guc_download_something_to_clear(void)
{
	return download_something_to_clear();
}

gboolean guc_download_new(gchar *file, guint32 size, 
			guint32 record_index, guint32 ip, guint16 port, gchar *guid, 
			gchar *hostname, gchar *sha1, time_t stamp, gboolean push,
			struct dl_file_info *fi, gnet_host_vec_t *proxies)
{
	return download_new(file, size, record_index, ip, port, guid, hostname, 
		sha1, stamp, push, fi, proxies);				
}

void guc_download_index_changed(guint32 ip, guint16 port, 
	gchar *guid, guint32 from, guint32 to)
{
	download_index_changed(ip, port, guid, from, to);
}

struct download *guc_src_get_download(gnet_src_t src_handle)
{
	return src_get_download(src_handle);
}

void guc_src_add_listener(src_listener_t cb, gnet_src_ev_t ev, 
    frequency_t t, guint32 interval)
{
	src_add_listener(cb, ev, t, interval);
}

void guc_src_remove_listener(src_listener_t cb, 
	gnet_src_ev_t ev)
{
	src_remove_listener(cb, ev);
}


/*	fileinfo interface functions (UI -> Core)*/
const gchar *guc_file_info_readable_filename
	(struct dl_file_info *fi)
{
	return file_info_readable_filename(fi);		
}

gnet_fi_info_t *guc_fi_get_info(gnet_fi_t fih)
{
	return fi_get_info(fih);
}

void guc_fi_free_info(gnet_fi_info_t *info)
{
	fi_free_info(info);
}

void guc_fi_get_status(gnet_fi_t fih, gnet_fi_status_t *s)
{
	fi_get_status(fih, s);
}

gchar **guc_fi_get_aliases(gnet_fi_t fih)
{
	return fi_get_aliases(fih);
}

void guc_fi_purge_by_handle_list(GSList *list)
{
	fi_purge_by_handle_list(list);
}

gboolean guc_fi_purge(gnet_fi_t fih)
{
	return fi_purge(fih);
}

void guc_fi_add_listener(fi_listener_t cb, gnet_fi_ev_t ev,
    frequency_t t, guint32 interval)
{
	fi_add_listener(cb, ev, t, interval);
}

void guc_fi_remove_listener(fi_listener_t cb, gnet_fi_ev_t ev)
{
	fi_remove_listener(cb, ev);
}

GSList *guc_fi_get_chunks(gnet_fi_t fih)
{
	return fi_get_chunks(fih);
}

void guc_fi_free_chunks(GSList *chunks)
{
	fi_free_chunks(chunks);
}

GSList *guc_fi_get_ranges(gnet_fi_t fih)
{
	return fi_get_ranges(fih);
}

void guc_fi_free_ranges(GSList *ranges)
{
	fi_free_ranges(ranges);
}

/*	gnet stats interface functions (UI -> Core)*/
void guc_gnet_stats_get(gnet_stats_t *stats)
{
	gnet_stats_get(stats);
}

void guc_gnet_get_bw_stats
	(gnet_bw_source type, gnet_bw_stats_t *stats)
{
	gnet_get_bw_stats(type, stats);
}


/*	hcache interface functions (UI -> Core)*/
void guc_hcache_clear_host_type(host_type_t type)
{
	hcache_clear_host_type(type);	
}

void guc_hcache_clear(hcache_type_t type)
{
	hcache_clear(type);
}

void guc_hcache_get_stats(hcache_stats_t *stats)
{
	hcache_get_stats(stats);
}

/*	HSEP interface functions (UI -> Core)*/
const gchar *guc_hsep_get_static_str(gint row, gint column)
{
	return hsep_get_static_str(row, column);
}

gint guc_hsep_get_table_size(void)
{
	return hsep_get_table_size();
}

void guc_hsep_get_non_hsep_triple(hsep_triple *tripledest)
{
	hsep_get_non_hsep_triple(tripledest);
}


void guc_hsep_add_global_table_listener(GCallback cb, 
	frequency_t t, guint32 interval)
{
	hsep_add_global_table_listener(cb, t, interval);
}

void guc_hsep_remove_global_table_listener(GCallback cb)
{
	hsep_remove_global_table_listener(cb);
}


/*	HTTP interface functions (UI -> Core)*/
const gchar *guc_http_range_to_gchar(const GSList *list)
{
	return http_range_to_gchar(list);
}

GSList * guc_http_range_merge
		(GSList *old_list, GSList *new_list)
{
	return http_range_merge(old_list, new_list);
}


/*	node interface functions (UI -> Core)*/
void guc_node_add_node_added_listener
	(node_added_listener_t l)
{
	node_add_node_added_listener(l);
}

void guc_node_add_node_removed_listener
	(node_removed_listener_t l)
{
	node_add_node_removed_listener(l);
}

void guc_node_add_node_info_changed_listener
	(node_info_changed_listener_t l)
{
	node_add_node_info_changed_listener(l);
}

void guc_node_add_node_flags_changed_listener
	(node_flags_changed_listener_t l)
{
	node_add_node_flags_changed_listener(l);
}

void guc_node_remove_node_added_listener
	(node_added_listener_t l)
{
	node_remove_node_added_listener(l);
}

void guc_node_remove_node_removed_listener
	(node_removed_listener_t l)
{
	node_remove_node_removed_listener(l);
}

void guc_node_remove_node_info_changed_listener
	(node_info_changed_listener_t l)
{
	node_remove_node_info_changed_listener(l);
}

void guc_node_remove_node_flags_changed_listener
	(node_flags_changed_listener_t l)
{
	node_remove_node_flags_changed_listener(l);
}

void guc_node_add(guint32 ip, guint16 port)
{
	node_add(ip, port);
}

void guc_node_remove_by_handle(gnet_node_t n)
{
	node_remove_by_handle(n);
}

void guc_node_remove_nodes_by_handle(GSList *node_list)
{
	node_remove_nodes_by_handle(node_list);
}

void guc_node_get_status
	(const gnet_node_t n, gnet_node_status_t *s)
{
	node_get_status(n, s);
}

gnet_node_info_t *guc_node_get_info(const gnet_node_t n)
{
	return node_get_info(n);
}

void guc_node_clear_info(gnet_node_info_t *info)
{
	node_clear_info(info);
}

void guc_node_free_info(gnet_node_info_t *info)
{
	node_free_info(info);
}

void guc_node_fill_flags
	(gnet_node_t n, gnet_node_flags_t *flags)
{
	node_fill_flags(n, flags);
}

void guc_node_fill_info
	(const gnet_node_t n, gnet_node_info_t *info)
{
	node_fill_info(n, info);
}


/*	parq interface functions (UI -> Core)*/
gint guc_get_parq_dl_position(const struct download *d)
{
	return get_parq_dl_position(d);
}

gint guc_get_parq_dl_queue_length(const struct download *d)
{
	return get_parq_dl_queue_length(d);
}

gint guc_get_parq_dl_eta(const struct download *d)
{
	return get_parq_dl_eta(d);
}

gint guc_get_parq_dl_retry_delay(const struct download *d)
{
	return get_parq_dl_retry_delay(d);
}


/*	search interface functions (UI -> Core)*/
void guc_search_update_items(gnet_search_t sh, guint32 items)
{
	search_update_items(sh, items);
}

guint32 guc_search_get_reissue_timeout(gnet_search_t sh)
{
	return search_get_reissue_timeout(sh);
}

void guc_search_set_reissue_timeout
	(gnet_search_t sh, guint32 timeout)
{
	search_set_reissue_timeout(sh, timeout);
}

gboolean guc_search_is_passive(gnet_search_t sh)
{
	return search_is_passive(sh);
}

gboolean guc_search_is_frozen(gnet_search_t sh)
{
	return search_is_frozen(sh);
}

gnet_search_t guc_search_new(const gchar *query, 
	guint16 minimum_speed, guint32 reissue_timeout, flag_t flags)
{
	return search_new(query, minimum_speed, reissue_timeout, flags);
}

void guc_search_reissue(gnet_search_t sh)
{
	search_reissue(sh);
}

void guc_search_close(gnet_search_t sh)
{
	search_close(sh);
}

void guc_search_start(gnet_search_t sh)
{
	search_start(sh);
}

void guc_search_stop(gnet_search_t sh)
{
	search_stop(sh);
}


/*	settings interface functions (UI -> Core)*/
const gchar *guc_settings_home_dir(void)
{
	return settings_home_dir();
}
const gchar *guc_settings_config_dir(void)
{
	return settings_config_dir();
}


/*	share interface functions (UI -> Core)*/
void guc_shared_dir_add(const gchar * path)
{
	shared_dir_add(path);
}

void guc_share_scan(void)
{
	share_scan();
}

guint64 guc_shared_files_scanned(void)
{
	return shared_files_scanned();
}

guint64 guc_shared_kbytes_scanned(void)
{
	return shared_kbytes_scanned();
}

void guc_share_add_search_request_listener(search_request_listener_t l)
{
	share_add_search_request_listener(l);	
}


void guc_share_remove_search_request_listener(search_request_listener_t l)
{
	share_remove_search_request_listener(l);
}

void guc_search_add_kept(gnet_search_t sh, guint32 kept)
{
	search_add_kept(sh, kept);
}

/*	upload interface functions (UI -> Core)*/
gnet_upload_info_t *guc_upload_get_info(gnet_upload_t uh)
{
	return upload_get_info(uh);
}

void guc_upload_free_info(gnet_upload_info_t *info)
{
	upload_free_info(info);
}

void guc_upload_get_status
	(gnet_upload_t uh, gnet_upload_status_t *si)
{
	upload_get_status( uh, si);
}

void guc_upload_kill(gnet_upload_t upload)
{
	upload_kill(upload);
}

void guc_upload_add_upload_added_listener
	(upload_added_listener_t l)
{
	upload_add_upload_added_listener(l);
}

void guc_upload_remove_upload_added_listener
	(upload_added_listener_t l)
{
	upload_remove_upload_added_listener(l);
}

void guc_upload_add_upload_removed_listener
	(upload_removed_listener_t l)
{
	upload_add_upload_removed_listener(l);
}

void guc_upload_remove_upload_removed_listener
	(upload_removed_listener_t l)
{
	upload_remove_upload_removed_listener(l);
}

void guc_upload_add_upload_info_changed_listener
	(upload_info_changed_listener_t l)
{
	upload_add_upload_info_changed_listener(l);
}

void guc_upload_remove_upload_info_changed_listener
	(upload_info_changed_listener_t l)
{
	upload_remove_upload_info_changed_listener(l);
}

/*	upload stats interface functions (UI -> Core)*/

void guc_upload_stats_prune_nonexistent(void)
{
	upload_stats_prune_nonexistent();
}

/*	version interface functions (UI -> Core)*/
const gchar *guc_version_get_version_string()
{
	return version_get_string();
}

/*	main interface functions (UI -> Core)*/
void guc_gtk_gnutella_exit(gint code)
{
	gtk_gnutella_exit(code);
}
