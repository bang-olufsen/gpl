
/*
 * Copyright (C) 2012 Intel Corporation.
 *
 * Author: Jens Georg <jensg@openismus.com>
 *
 * Rygel is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Rygel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

using GUPnP;

/**
 * This interface abstracts global states that are not dependant on the
 * RygelMediaPlayer.
 *
 * These states are:
 * # URI
 * # MetaData
 * # Number of tracks
 * # Current track
 * # Playback state
 *
 * In case of playlists this interface will also provide control of the player.
 * It needs to proxy the playback state to react on end of item to be able to
 * switch to the next item.
 */
public interface Rygel.PlayerControllerInterface : Object {
    /* public properties */
    [CCode (notify = false)]
    public abstract string playback_state { get; set; }
    public abstract uint n_tracks { get; set; }
    public abstract uint track { get; set; }
    public abstract string uri { get; set; }
    public abstract string metadata { get; set; }
    public abstract string track_uri { owned get; set; }
    public abstract string track_metadata { owned get; set; }
    public abstract string next_uri { get; set; }
    public abstract string next_metadata { get; set; }
    public abstract string current_transport_actions { owned get; }
    public abstract string manufacture_id { get; set; }
    public abstract string device_id { get; set; }
    public abstract string auth_seed { get; set; }
    public abstract string auth_code { get; set; }
    public abstract string network_ssid { get; set; }
    public abstract string network_key { get; set; }
    public abstract void set_network (string ssid, string key);
    public abstract bool next ();
    public abstract bool previous ();
    public abstract void calc_auth_code (string seed);
    public abstract void set_single_play_uri (string uri, string metadata, string? mime, string? features);
    public abstract void set_playlist_uri (string uri, string metadata, MediaCollection collection);
    public abstract void set_next_single_play_uri (string uri, string metadata, string? mime, string? features);
    public abstract void set_next_playlist_uri (string uri, string metadata, MediaCollection collection);
    public abstract void set_qplay_queue_uri (string uri, string metadata, string? mime, string? features);
    public abstract string queue_id { get; set; }
    public abstract string starting_index { get; set; }
    public abstract string tracks_meta_data { get; set; }
    public abstract int insert_tracks (string queue_id, string starting_index, string  tracks_meta_data);
    public abstract int remove_tracks (string queue_id, string starting_index, string  number_of_tracks);
    public abstract void get_tracks_info (string starting_index, string number_of_tracks);
    public abstract int set_tracks_info (string queue_id, string starting_index, string next_index, string tracks_meta_data);
    public abstract string next_index { get; set; }
    public abstract string tracks_count { get; set; }
    public abstract string max_tracks { get; set; }
    public abstract void calc_tracks_count ();
    public abstract void calc_max_tracks ();
    public abstract string play_mode { get; set; }
    /// Total duration of the current playlist in microseconds
    public abstract int64 media_duration { get; }
    /// The duration as a human-readable string, in HH:MM:SS format
    public string media_duration_as_str {
        owned get {
            return TimeUtils.time_to_string (media_duration);
        }
    }
}
