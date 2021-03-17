/*
 * Copyright (C) 2008 Zeeshan Ali <zeenix@gmail.com>.
 *
 * Author: Zeeshan Ali <zeenix@gmail.com>
 *
 * This file is part of Rygel.
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
using Rygel;

/**
 * Basic implementation of QPlay service version 2.
 */
internal class QPlay: Service {
    public const string UPNP_ID =
                    "urn:tencent-com:serviceId:QPlay";
    // UPnP requires that points replaced by hyphens in domain names
    public const string UPNP_TYPE =
                    "urn:schemas-tencent-com:service:QPlay:1";
    public const string DESCRIPTION_PATH =
                    "xml/QPlayDescription.xml";

    private PlayerControllerInterface controller;

    public override void constructed () {
        base.constructed ();

        var plugin = this.root_device.resource_factory as MediaRendererPlugin;
        this.controller = plugin.get_controller ();
        this.action_invoked["SetNetwork"].connect (this.set_network_cb);
        this.action_invoked["QPlayAuth"].connect (this.set_qplay_auth_cb);
        this.action_invoked["InsertTracks"].connect (this.insert_tracks_cb);
        this.action_invoked["RemoveTracks"].connect (this.remove_tracks_cb);
        this.action_invoked["GetTracksInfo"].connect (this.get_tracks_info_cb);
        this.action_invoked["SetTracksInfo"].connect (this.set_tracks_info_cb);
        this.action_invoked["GetTracksCount"].connect (this.get_tracks_count_cb);
        this.action_invoked["GetMaxTracks"].connect (this.get_max_tracks_cb);

        this.query_variable["FakeState"].connect (this.query_fake_state_cb);
    }

    private void query_fake_state_cb (Service        registrar,
                                      string         variable,
                                      ref GLib.Value value) {
        value.init (typeof (int));
        value.set_int (0);
    }

    /* SetNetwork, QPlayAuth and FakeAction action implementations (fake) */
    private void set_network_cb (Service       registrar,
                                 ServiceAction action) {
        if (action.get_argument_count () != 2) {
            action.return_error (402, _("Invalid argument"));
            return;
        }

        string ssid, key;
        action.get ("SSID", typeof (string), out ssid);
        action.get ("Key", typeof (string), out key);

        if (ssid == null || ssid == "") {
            action.return_error (600, _("Invocation Error"));
            return;
        }

        this.controller.network_key = key;
        this.controller.network_ssid = ssid;
        this.controller.set_network (ssid, key);

        action.return ();
    }

    private void set_qplay_auth_cb (Service       registrar,
                                    ServiceAction action) {
        if (action.get_argument_count () != 1) {
            action.return_error (402, _("Invalid argument"));
            return;
        }

        string seed;
        action.get ("Seed", typeof (string), out seed);

        if (seed == null || seed == "") {
            action.return_error (600, _("Invocation Error"));
            return;
        }
        this.controller.calc_auth_code (seed);

        action.set ("Code", typeof (string), this.controller.auth_code);
        action.set ("MID", typeof (string), this.controller.manufacture_id);
        action.set ("DID", typeof (string), this.controller.device_id);

        action.return ();
    }

    private void insert_tracks_cb (Service       registrar,
                                   ServiceAction action) {
        if (action.get_argument_count () != 3) {
            action.return_error (402, _("Invalid argument"));
            return;
        }

        string queue_id, starting_index, tracks_meta_data;

        action.get ("QueueID", typeof(string), out queue_id);
        action.get ("StartingIndex", typeof(string), out starting_index);
        action.get ("TracksMetaData", typeof(string), out tracks_meta_data);

        if (queue_id == null || queue_id == "" || starting_index == null ||
            starting_index == "" || tracks_meta_data == null || tracks_meta_data == "") {
            action.return_error (600, _("Invocation Error"));
            return;
        }

        if (queue_id != this.controller.queue_id) {
            action.return_error (718, _("invalid QueueID"));
            return;
        }

        int number_of_success = this.controller.insert_tracks (queue_id, starting_index, tracks_meta_data);

        action.set ("NumberOfSuccess", typeof (string), number_of_success.to_string());

        action.return ();
     }

     private void remove_tracks_cb (Service       registrar,
                                     ServiceAction action) {
        if (action.get_argument_count () != 3) {
            action.return_error (402, _("Invalid argument"));
            return;
        }

        string queue_id, starting_index, number_of_tracks;

        action.get ("QueueID", typeof(string), out queue_id);
        action.get ("StartingIndex", typeof(string), out starting_index);
        action.get ("NumberOfTracks", typeof(string), out number_of_tracks);

        if (queue_id == null || queue_id == "" || starting_index == null ||
            starting_index == "" || number_of_tracks == null || number_of_tracks == "") {
            action.return_error (600, _("Invocation Error"));
            return;
        }

        if (queue_id != this.controller.queue_id) {
            action.return_error (718, _("invalid QueueID"));
            return;
        }

        int number_of_success = this.controller.remove_tracks (queue_id, starting_index, number_of_tracks);

        action.set ("NumberOfSuccess", typeof (string), number_of_success.to_string());

        action.return ();
     }

     private void get_tracks_info_cb (Service       registrar,
                                      ServiceAction action) {
        if (action.get_argument_count () != 2) {
            action.return_error (402, _("Invalid argument"));
            return;
        }

        string starting_index, number_of_tracks;

        action.get ("StartingIndex", typeof(string), out starting_index);
        action.get ("NumberOfTracks", typeof(string), out number_of_tracks);

        if (starting_index == null || starting_index == "" ||
            number_of_tracks == null || number_of_tracks == "" ) {
            action.return_error (600, _("Invocation Error"));
            return;
        }

        this.controller.get_tracks_info (starting_index, number_of_tracks);

        action.set ("TracksMetaData", typeof (string), this.controller.tracks_meta_data);

        action.return ();
     }

     private void set_tracks_info_cb (Service       registrar,
                                     ServiceAction action) {
        if (action.get_argument_count () != 4) {
            action.return_error (402, _("Invalid argument"));
            return;
        }

        string queue_id, starting_index, next_index, tracks_meta_data;

        action.get ("QueueID", typeof(string), out queue_id);
        action.get ("StartingIndex", typeof(string), out starting_index);
        action.get ("NextIndex", typeof(string), out next_index);
        action.get ("TracksMetaData", typeof(string), out tracks_meta_data);

        if (queue_id == null || queue_id == "" || starting_index == null ||
            starting_index == "" || next_index == null || next_index == "" ||
            tracks_meta_data == null || tracks_meta_data == "" ) {
            action.return_error (600, _("Invocation Error"));
            return;
        }

        if (queue_id != this.controller.queue_id) {
            action.return_error (718, _("invalid QueueID"));
            return;
        }

        int number_of_success = this.controller.set_tracks_info (queue_id, starting_index, next_index, tracks_meta_data);

        action.set ("NumberOfSuccess", typeof (string), number_of_success.to_string());

        action.return ();
     }

     private void get_tracks_count_cb (Service       registrar,
                                       ServiceAction action) {

        this.controller.calc_tracks_count ();

        action.set ("NrTracks", typeof (string), this.controller.tracks_count);

        action.return ();
     }

     private void get_max_tracks_cb (Service       registrar,
                                     ServiceAction action) {

        this.controller.calc_max_tracks ();

        action.set ("MaxTracks", typeof (string), this.controller.max_tracks);

        action.return ();
     }
}
