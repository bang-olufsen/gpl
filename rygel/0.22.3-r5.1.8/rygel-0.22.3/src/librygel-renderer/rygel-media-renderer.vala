/*
 * Copyright (C) 2012 Openismus GmbH.
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

internal class Plugin : Rygel.MediaRendererPlugin {
    private Rygel.MediaPlayer player;

    public Plugin (Rygel.MediaPlayer        player,
                   Rygel.PluginCapabilities capabilities,
                   Rygel.PlayerControllerInterface?  controller = null) {
        base ("Beo_ASE", "Beo_ASE", null, capabilities,
		controller);
        this.player = player;
    }

    public override Rygel.MediaPlayer? get_player () {
        return this.player;
    }
}

/**
 * This class may be used to implement in-process UPnP-AV media renderers.
 *
 * Call rygel_media_device_add_interface() on the RygelMediaRenderer to allow it
 * to render media from that network interface.
 *
 * See the
 * <link linkend="implementing-renderers">Implementing Renderers</link> section.
 */
public class Rygel.MediaRenderer : MediaDevice {
    public unowned MediaPlayer player { construct; private get; }

    /**
     * Create a RygelMediaRenderer to render content via a RygelMediaPlayer.
     */
    public MediaRenderer (string title,
                          string? model_name,
                          string? model_number,
                          MediaPlayer player,
                          PluginCapabilities capabilities =
                                        PluginCapabilities.NONE,
                          PlayerControllerInterface? controller = null) {
        Object (title: title,
                model_name: model_name,
                model_number: model_number,
                player: player,
                capabilities: capabilities,
                controller: controller);
    }

    public override void constructed () {
        base.constructed ();

        if (this.plugin == null) {
            this.plugin = new global::Plugin (this.player, this.capabilities, this.controller);
        }
        this.plugin.title = this.title;
        this.plugin.model_name = this.model_name;
        this.plugin.model_number = this.model_number;
    }
    public unowned PlayerControllerInterface controller { construct; private get; }
}
