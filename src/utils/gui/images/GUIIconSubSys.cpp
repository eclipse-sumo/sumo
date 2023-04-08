/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIIconSubSys.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Andreas Gaubatz
/// @date    Mon, 08.03.2004
///
// Helper for icons loading and usage
/****************************************************************************/
#include <config.h>

#include <utils/common/UtilExceptions.h>

#include "GUIIcons.h"
#include "GUIIconSubSys.h"

#include "sumo_icon64.xpm"
#include "sumo_icon16.xpm"
#include "sumo_logo.xpm"

#include "netgenerate.xpm"
#include "netedit_icon64.xpm"
#include "netedit_icon16.xpm"

#include "netgenerate_grid.xpm"
#include "netgenerate_spider.xpm"
#include "netgenerate_randomgrid.xpm"
#include "netgenerate_random.xpm"

#include "empty.xpm"
#include "newnet.xpm"
#include "play.xpm"
#include "stop.xpm"
#include "step.xpm"
#include "new_window.xpm"
#include "new_window_3d.xpm"

#include "open.xpm"
#include "openadditionals.xpm"
#include "openbmpdialog.xpm"
#include "opennet.xpm"
#include "openmeandatas.xpm"
#include "openneteditconfig.xpm"
#include "openshapes.xpm"
#include "opensumoconfig.xpm"
#include "opentls.xpm"

#include "save.xpm"
#include "save_sumoconfig.xpm"
#include "save_neteditconfig.xpm"
#include "save_multiple.xpm"
#include "save_networkelements.xpm"
#include "save_aditionalelements.xpm"
#include "save_demandelements.xpm"
#include "save_dataelements.xpm"
#include "save_meandataelements.xpm"
#include "save_database.xpm"

#include "app_tracker.xpm"
#include "app_finder.xpm"
#include "app_breakpoints.xpm"
#include "app_tlstracker.xpm"
#include "app_table.xpm"
#include "app_selector.xpm"

#include "locate.xpm"
#include "locate_junction.xpm"
#include "locate_edge.xpm"
#include "locate_walkingarea.xpm"
#include "locate_vehicle.xpm"
#include "locate_route.xpm"
#include "locate_stop.xpm"
#include "locate_person.xpm"
#include "locate_container.xpm"
#include "locate_tls.xpm"
#include "locate_add.xpm"
#include "locate_poi.xpm"
#include "locate_poly.xpm"

#include "tool_python.xpm"
#include "tool_emissions.xpm"
#include "tool_citybrain.xpm"
#include "tool_gtfs.xpm"
#include "tool_vissim.xpm"
#include "tool_visum.xpm"
#include "tool_import.xpm"
#include "tool_output.xpm"
#include "tool_turndefs.xpm"
#include "tool_xml.xpm"

#include "green_container.xpm"
#include "yellow_container.xpm"
#include "green_edge.xpm"
#include "yellow_edge.xpm"
#include "green_vehicle.xpm"
#include "yellow_vehicle.xpm"
#include "green_person.xpm"
#include "yellow_person.xpm"

#include "colorwheel.xpm"
#include "removedb.xpm"
#include "recenter_view.xpm"
#include "allow_rotation.xpm"
#include "show_tooltips_view.xpm"
#include "show_tooltips_menu.xpm"
#include "magnify.xpm"
#include "zoomstyle.xpm"
#include "yes.xpm"
#include "no.xpm"

#include "extract_streets.xpm"
#include "dilate.xpm"
#include "erode.xpm"
#include "opening.xpm"
#include "closing.xpm"
#include "erase_stains.xpm"
#include "close_gaps.xpm"
#include "skeletonize.xpm"
#include "rarify.xpm"
#include "create_graph.xpm"
#include "eyedrop.xpm"
#include "rubber1x.xpm"
#include "rubber2x.xpm"
#include "rubber3x.xpm"
#include "rubber4x.xpm"
#include "rubber5x.xpm"
#include "paintbrush1x.xpm"
#include "paintbrush2x.xpm"
#include "paintbrush3x.xpm"
#include "paintbrush4x.xpm"
#include "paintbrush5x.xpm"
#include "edit_graph.xpm"
#include "full_screen.xpm"

#include "flag.cpp"
#include "flag_plus.cpp"
#include "flag_minus.cpp"
#include "windows_cascade.xpm"
#include "windows_tile_vertically.xpm"
#include "windows_tile_horizontally.xpm"
#include "reload.xpm"
#include "manip.cpp"
#include "camera.xpm"

#include "undo.xpm"
#include "redo.xpm"
#include "undolist.xpm"
#include "lock.xpm"
#include "unlock.xpm"
#include "lock_selected.xpm"
#include "unlock_selected.xpm"
#include "add.xpm"
#include "remove.xpm"
#include "collapse.xpm"
#include "uncollapse.xpm"
#include "extend.xpm"
#include "current.xpm"

#include "arrowup.xpm"
#include "arrowdown.xpm"
#include "arrowleft.xpm"
#include "arrowright.xpm"

#include "bigarrowleft.xpm"
#include "bigarrowright.xpm"

#include "frontelement.xpm"
#include "simplifynetwork.xpm"
#include "computepathmanager.xpm"

#include "checkbox_applytoallphases.xpm"
#include "checkbox_askformerge.xpm"
#include "checkbox_autoselectjunctions.xpm"
#include "checkbox_bubbles.xpm"
#include "checkbox_chain.xpm"
#include "checkbox_elevation.xpm"
#include "checkbox_grid.xpm"
#include "checkbox_drawjunctionshapes.xpm"
#include "checkbox_hideconnections.xpm"
#include "checkbox_showsubadditionals.xpm"
#include "checkbox_showtazelements.xpm"
#include "checkbox_hidenoninspecteddemandelements.xpm"
#include "checkbox_hideshapes.xpm"
#include "checkbox_lockcontainer.xpm"
#include "checkbox_lockperson.xpm"
#include "checkbox_selectededges.xpm"
#include "checkbox_showadditionals.xpm"
#include "checkbox_showconnections.xpm"
#include "checkbox_showcontainerplans.xpm"
#include "checkbox_showdemandelements.xpm"
#include "checkbox_showoverlappedroutes.xpm"
#include "checkbox_showpersonplans.xpm"
#include "checkbox_showshapes.xpm"
#include "checkbox_showtrips.xpm"
#include "checkbox_spreadvehicle.xpm"
#include "checkbox_tazdrawfill.xpm"
#include "checkbox_tazreldrawing.xpm"
#include "checkbox_tazrelonlyfrom.xpm"
#include "checkbox_tazrelonlyto.xpm"
#include "checkbox_twoway.xpm"

#include "lanebike.xpm"
#include "lanebus.xpm"
#include "lanepedestrian.xpm"
#include "lanegreenverge.xpm"

#include "cut.xpm"
#include "copy.xpm"
#include "paste.xpm"

#include "select.xpm"
#include "unselect.xpm"

#include "join.xpm"
#include "disjoin.xpm"

#include "ext.xpm"
#include "tracker.xpm"
#include "hall_of_fame.xpm"
#include "clear_message_window.xpm"

#include "supermodenetwork.xpm"
#include "supermodedemand.xpm"
#include "supermodedata.xpm"

#include "modeadditional.xpm"
#include "modeconnection.xpm"
#include "modecreateedge.xpm"
#include "modecrossing.xpm"
#include "modetaz.xpm"
#include "modedelete.xpm"
#include "modeinspect.xpm"
#include "modemove.xpm"
#include "modeselect.xpm"
#include "modetrafficlight.xpm"
#include "modeshape.xpm"
#include "modeprohibition.xpm"
#include "modewire.xpm"

#include "modecontainer.xpm"
#include "modecontainerplan.xpm"
#include "modeperson.xpm"
#include "modepersonplan.xpm"
#include "moderoute.xpm"
#include "modestop.xpm"
#include "modevehicle.xpm"
#include "modetype.xpm"

#include "modeedgedata.xpm"
#include "modeedgereldata.xpm"
#include "modetazreldata.xpm"
#include "modemeandata.xpm"
#include "meandataedge.xpm"
#include "meandatalane.xpm"

#include "tlsphasedefault.xpm"
#include "tlsphasecopy.xpm"
#include "tlsphaseallred.xpm"
#include "tlsphaseallyellow.xpm"
#include "tlsphaseallgreen.xpm"
#include "tlsphaseallgreenpriority.xpm"

#include "computejunctions.xpm"
#include "cleanjunctions.xpm"
#include "joinjunctions.xpm"
#include "computedemand.xpm"
#include "cleanroutes.xpm"
#include "joinroutes.xpm"
#include "adjustpersonplans.xpm"
#include "options.xpm"

#include "junction.xpm"
#include "edgetype.xpm"
#include "lanetype.xpm"
#include "edge.xpm"
#include "lane.xpm"
#include "connection.xpm"
#include "prohibition.xpm"
#include "crossing.xpm"
#include "walkingarea.xpm"

#include "busstop.xpm"
#include "trainstop.xpm"
#include "access.xpm"
#include "chargingstation.xpm"
#include "containerstop.xpm"
#include "parkingarea.xpm"
#include "parkingspace.xpm"
#include "e1.xpm"
#include "e1instant.xpm"
#include "e2.xpm"
#include "e3.xpm"
#include "e3entry.xpm"
#include "e3exit.xpm"
#include "rerouter.xpm"
#include "routeprobe.xpm"
#include "vaporizer.xpm"
#include "variablespeedsign.xpm"
#include "calibrator.xpm"
#include "rerouterinterval.xpm"
#include "vssstep.xpm"
#include "closingreroute.xpm"
#include "closinglanereroute.xpm"
#include "destprobreroute.xpm"
#include "parkingzonereroute.xpm"
#include "routeprobreroute.xpm"

#include "tractionsubstation.xpm"
#include "overheadwire.xpm"
#include "overheadwire_clamp.xpm"

#include "poly.xpm"
#include "poi.xpm"
#include "poilane.xpm"
#include "poigeo.xpm"

#include "taz.xpm"
#include "tazedge.xpm"

#include "route.xpm"
#include "vtype.xpm"
#include "vtypedistribution.xpm"
#include "vehicle.xpm"
#include "trip.xpm"
#include "tripjunctions.xpm"
#include "flow.xpm"
#include "flowjunctions.xpm"
#include "routeflow.xpm"
#include "stopelement.xpm"
#include "waypoint.xpm"
#include "person.xpm"
#include "personflow.xpm"
#include "persontripfromto.xpm"
#include "persontripbusstop.xpm"
#include "persontripjunctions.xpm"
#include "walkedges.xpm"
#include "walkfromto.xpm"
#include "walkbusstop.xpm"
#include "walkroute.xpm"
#include "walkjunctions.xpm"
#include "ridefromto.xpm"
#include "ridebusstop.xpm"
#include "container.xpm"
#include "containerflow.xpm"
#include "transhipcontainerstop.xpm"
#include "transhipedges.xpm"
#include "transhipfromto.xpm"
#include "transportcontainerstop.xpm"
#include "transportfromto.xpm"


#include "dataset.xpm"
#include "datainterval.xpm"
#include "edgedata.xpm"
#include "edgereldata.xpm"
#include "tazreldata.xpm"

#include "vclass_ignoring.xpm"
#include "vclass_private.xpm"
#include "vclass_emergency.xpm"
#include "vclass_authority.xpm"
#include "vclass_army.xpm"
#include "vclass_vip.xpm"
#include "vclass_passenger.xpm"
#include "vclass_hov.xpm"
#include "vclass_taxi.xpm"
#include "vclass_bus.xpm"
#include "vclass_coach.xpm"
#include "vclass_delivery.xpm"
#include "vclass_truck.xpm"
#include "vclass_trailer.xpm"
#include "vclass_tram.xpm"
#include "vclass_rail_urban.xpm"
#include "vclass_rail.xpm"
#include "vclass_rail_electric.xpm"
#include "vclass_rail_fast.xpm"
#include "vclass_motorcycle.xpm"
#include "vclass_moped.xpm"
#include "vclass_bicycle.xpm"
#include "vclass_pedestrian.xpm"
#include "vclass_evehicle.xpm"
#include "vclass_ship.xpm"
#include "vclass_custom1.xpm"
#include "vclass_custom2.xpm"

#include "vclass_small_ignoring.xpm"
#include "vclass_small_private.xpm"
#include "vclass_small_emergency.xpm"
#include "vclass_small_authority.xpm"
#include "vclass_small_army.xpm"
#include "vclass_small_vip.xpm"
#include "vclass_small_passenger.xpm"
#include "vclass_small_hov.xpm"
#include "vclass_small_taxi.xpm"
#include "vclass_small_bus.xpm"
#include "vclass_small_coach.xpm"
#include "vclass_small_delivery.xpm"
#include "vclass_small_truck.xpm"
#include "vclass_small_trailer.xpm"
#include "vclass_small_tram.xpm"
#include "vclass_small_rail_urban.xpm"
#include "vclass_small_rail.xpm"
#include "vclass_small_rail_electric.xpm"
#include "vclass_small_rail_fast.xpm"
#include "vclass_small_motorcycle.xpm"
#include "vclass_small_moped.xpm"
#include "vclass_small_bicycle.xpm"
#include "vclass_small_pedestrian.xpm"
#include "vclass_small_evehicle.xpm"
#include "vclass_small_ship.xpm"
#include "vclass_small_custom1.xpm"
#include "vclass_small_custom2.xpm"

#include "vshape_pedestrian.xpm"
#include "vshape_bicycle.xpm"
#include "vshape_moped.xpm"
#include "vshape_motorcycle.xpm"
#include "vshape_passenger.xpm"
#include "vshape_passenger_sedan.xpm"
#include "vshape_passenger_hatchback.xpm"
#include "vshape_passenger_wagon.xpm"
#include "vshape_passenger_van.xpm"
#include "vshape_taxi.xpm"
#include "vshape_delivery.xpm"
#include "vshape_truck.xpm"
#include "vshape_truck_semitrailer.xpm"
#include "vshape_truck_1trailer.xpm"
#include "vshape_bus.xpm"
#include "vshape_bus_coach.xpm"
#include "vshape_bus_flexible.xpm"
#include "vshape_bus_trolley.xpm"
#include "vshape_rail.xpm"
#include "vshape_rail_car.xpm"
#include "vshape_rail_cargo.xpm"
#include "vshape_e_vehicle.xpm"
#include "vshape_ant.xpm"
#include "vshape_ship.xpm"
#include "vshape_emergency.xpm"
#include "vshape_firebrigade.xpm"
#include "vshape_police.xpm"
#include "vshape_rickshaw.xpm"
#include "vshape_scooter.xpm"
#include "vshape_aircraft.xpm"
#include "vshape_unknown.xpm"

#include "accept.xpm"
#include "cancel.xpm"
#include "correct.xpm"
#include "incorrect.xpm"
#include "reset.xpm"
#include "warning.xpm"
#include "danger.xpm"
#include "sum.xpm"
#include "back.xpm"

#include "grid.xpm"
#include "grid1.xpm"
#include "grid2.xpm"
#include "grid3.xpm"

#include "geohack.xpm"
#include "googlesat.xpm"
#include "osm.xpm"

#include "languages/de.xpm"
#include "languages/es.xpm"
#include "languages/fr.xpm"
#include "languages/it.xpm"
#include "languages/en.xpm"
#include "languages/hu.xpm"
#include "languages/tr.xpm"
#include "languages/zh.xpm"
#include "languages/zht.xpm"

// ===========================================================================
// static member variable definitions
// ===========================================================================

GUIIconSubSys* GUIIconSubSys::myInstance = nullptr;

// ===========================================================================
// member definitions
// ===========================================================================

GUIIconSubSys::GUIIconSubSys(FXApp* a) {
    // build icons
    myIcons[GUIIcon::SUMO] = new FXXPMIcon(a, sumo_icon64_xpm);
    myIcons[GUIIcon::SUMO_MINI] = new FXXPMIcon(a, sumo_icon16_xpm);
    myIcons[GUIIcon::SUMO_LOGO] = new FXXPMIcon(a, sumo_logo_xpm);

    myIcons[GUIIcon::NETGENERATE] = new FXXPMIcon(a, netgenerate_xpm);
    myIcons[GUIIcon::NETEDIT] = new FXXPMIcon(a, netedit_icon64_xpm);
    myIcons[GUIIcon::NETEDIT_MINI] = new FXXPMIcon(a, netedit_icon16_xpm);

    myIcons[GUIIcon::NETGENERATE_GRID] = new FXXPMIcon(a, netgenerate_grid_xpm);
    myIcons[GUIIcon::NETGENERATE_SPIDER] = new FXXPMIcon(a, netgenerate_spider_xpm);
    myIcons[GUIIcon::NETGENERATE_RANDOMGRID] = new FXXPMIcon(a, netgenerate_randomgrid_xpm);
    myIcons[GUIIcon::NETGENERATE_RANDOM] = new FXXPMIcon(a, netgenerate_random_xpm);

    myIcons[GUIIcon::EMPTY] = new FXXPMIcon(a, empty_xpm);
    myIcons[GUIIcon::NEW_NET] = new FXXPMIcon(a, newnet_xpm);
    myIcons[GUIIcon::RELOAD] = new FXXPMIcon(a, reload_xpm);
    myIcons[GUIIcon::CLOSE] = new FXXPMIcon(a, empty_xpm);  /** temporal **/
    myIcons[GUIIcon::HELP] = new FXXPMIcon(a, empty_xpm);   /** temporal **/
    myIcons[GUIIcon::START] = new FXXPMIcon(a, play_xpm);
    myIcons[GUIIcon::STOP] = new FXXPMIcon(a, stop_xpm);
    myIcons[GUIIcon::STEP] = new FXXPMIcon(a, step_xpm);
    myIcons[GUIIcon::MICROVIEW] = new FXXPMIcon(a, new_window_xpm);
    myIcons[GUIIcon::OSGVIEW] = new FXXPMIcon(a, new_window_3d_xpm);
    myIcons[GUIIcon::RECENTERVIEW] = new FXXPMIcon(a, recenter_view_xpm);
    myIcons[GUIIcon::ALLOWROTATION] = new FXXPMIcon(a, allow_rotation_xpm);
    myIcons[GUIIcon::EDITVIEWPORT] = new FXXPMIcon(a, magnify_xpm);
    myIcons[GUIIcon::ZOOMSTYLE] = new FXXPMIcon(a, zoomstyle_xpm);
    myIcons[GUIIcon::FULL_SCREEN] = new FXXPMIcon(a, full_screen_xpm);
    myIcons[GUIIcon::HALL_OF_FAME] = new FXXPMIcon(a, hall_of_fame_xpm);   /** temporal? **/
    myIcons[GUIIcon::CLEARMESSAGEWINDOW] = new FXXPMIcon(a, clear_message_window_xpm);

    myIcons[GUIIcon::OPEN] = new FXXPMIcon(a, open_xpm);
    myIcons[GUIIcon::OPEN_NETEDITCONFIG] = new FXXPMIcon(a, open_neteditconfig_xpm);
    myIcons[GUIIcon::OPEN_SUMOCONFIG] = new FXXPMIcon(a, open_sumoconfig_xpm);
    myIcons[GUIIcon::OPEN_NET] = new FXXPMIcon(a, open_net_xpm);
    myIcons[GUIIcon::OPEN_SHAPES] = new FXXPMIcon(a, open_shapes_xpm);
    myIcons[GUIIcon::OPEN_ADDITIONALS] = new FXXPMIcon(a, open_additionals_xpm);
    myIcons[GUIIcon::OPEN_MEANDATAS] = new FXXPMIcon(a, open_meandatas_xpm);
    myIcons[GUIIcon::OPEN_TLSPROGRAMS] = new FXXPMIcon(a, open_tls_xpm);
    myIcons[GUIIcon::OPEN_BMPDIALOG] = new FXXPMIcon(a, open_bmpdialog_xpm);

    myIcons[GUIIcon::SAVE] = new FXXPMIcon(a, save_xpm);
    myIcons[GUIIcon::SAVE_SUMOCONFIG] = new FXXPMIcon(a, save_sumoconfig_xpm);
    myIcons[GUIIcon::SAVE_NETEDITCONFIG] = new FXXPMIcon(a, save_neteditconfig_xpm);
    myIcons[GUIIcon::SAVE_MULTIPLE] = new FXXPMIcon(a, save_multiple_xpm);
    myIcons[GUIIcon::SAVE_NETWORKELEMENTS] = new FXXPMIcon(a, save_networkelements_xpm);
    myIcons[GUIIcon::SAVE_ADDITIONALELEMENTS] = new FXXPMIcon(a, save_additionalelements_xpm);
    myIcons[GUIIcon::SAVE_DEMANDELEMENTS] = new FXXPMIcon(a, save_demandelements_xpm);
    myIcons[GUIIcon::SAVE_DATAELEMENTS] = new FXXPMIcon(a, save_dataelements_xpm);
    myIcons[GUIIcon::SAVE_MEANDATAELEMENTS] = new FXXPMIcon(a, save_meandataelements_xpm);
    myIcons[GUIIcon::SAVE_DATABASE] = new FXXPMIcon(a, save_database_xpm);

    myIcons[GUIIcon::CUT] = new FXXPMIcon(a, cut_xpm);
    myIcons[GUIIcon::COPY] = new FXXPMIcon(a, copy_xpm);
    myIcons[GUIIcon::PASTE] = new FXXPMIcon(a, paste_xpm);

    myIcons[GUIIcon::SELECT] = new FXXPMIcon(a, select_xpm);
    myIcons[GUIIcon::UNSELECT] = new FXXPMIcon(a, unselect_xpm);

    myIcons[GUIIcon::JOIN] = new FXXPMIcon(a, join_xpm);
    myIcons[GUIIcon::DISJOIN] = new FXXPMIcon(a, disjoin_xpm);

    myIcons[GUIIcon::APP_TRACKER] = new FXXPMIcon(a, app_tracker_xpm);
    myIcons[GUIIcon::APP_FINDER] = new FXXPMIcon(a, app_finder_xpm);
    myIcons[GUIIcon::APP_BREAKPOINTS] = new FXXPMIcon(a, app_breakpoints_xpm);
    myIcons[GUIIcon::APP_TLSTRACKER] = new FXXPMIcon(a, app_tlstracker_xpm);
    myIcons[GUIIcon::APP_TABLE] = new FXXPMIcon(a, app_table_xpm);
    myIcons[GUIIcon::APP_SELECTOR] = new FXXPMIcon(a, app_selector_xpm);

    myIcons[GUIIcon::LOCATE] = new FXXPMIcon(a, locate_xpm);
    myIcons[GUIIcon::LOCATEJUNCTION] = new FXXPMIcon(a, locate_junction_xpm);
    myIcons[GUIIcon::LOCATEEDGE] = new FXXPMIcon(a, locate_edge_xpm);
    myIcons[GUIIcon::LOCATEWALKINGAREA] = new FXXPMIcon(a, locate_walkingarea_xpm);
    myIcons[GUIIcon::LOCATEVEHICLE] = new FXXPMIcon(a, locate_vehicle_xpm);
    myIcons[GUIIcon::LOCATEROUTE] = new FXXPMIcon(a, locate_route_xpm);
    myIcons[GUIIcon::LOCATESTOP] = new FXXPMIcon(a, locate_stop_xpm);
    myIcons[GUIIcon::LOCATEPERSON] = new FXXPMIcon(a, locate_person_xpm);
    myIcons[GUIIcon::LOCATECONTAINER] = new FXXPMIcon(a, locate_container_xpm);
    myIcons[GUIIcon::LOCATETLS] = new FXXPMIcon(a, locate_tls_xpm);
    myIcons[GUIIcon::LOCATEADD] = new FXXPMIcon(a, locate_add_xpm);
    myIcons[GUIIcon::LOCATEPOI] = new FXXPMIcon(a, locate_poi_xpm);
    myIcons[GUIIcon::LOCATEPOLY] = new FXXPMIcon(a, locate_poly_xpm);

    myIcons[GUIIcon::TOOL_PYTHON] = new FXXPMIcon(a, tool_python_xpm);
    myIcons[GUIIcon::TOOL_EMISSIONS] = new FXXPMIcon(a, tool_emissions_xpm);
    myIcons[GUIIcon::TOOL_CITYBRAIN] = new FXXPMIcon(a, tool_citybrain_xpm);
    myIcons[GUIIcon::TOOL_GTFS] = new FXXPMIcon(a, tool_gtfs_xpm);
    myIcons[GUIIcon::TOOL_VISSIM] = new FXXPMIcon(a, tool_vissim_xpm);
    myIcons[GUIIcon::TOOL_VISUM] = new FXXPMIcon(a, tool_visum_xpm);
    myIcons[GUIIcon::TOOL_IMPORT] = new FXXPMIcon(a, tool_import_xpm);
    myIcons[GUIIcon::TOOL_OUTPUT] = new FXXPMIcon(a, tool_output_xpm);
    myIcons[GUIIcon::TOOL_TURNDEFS] = new FXXPMIcon(a, tool_turndefs_xpm);
    myIcons[GUIIcon::TOOL_XML] = new FXXPMIcon(a, tool_xml_xpm);

    myIcons[GUIIcon::GREENCONTAINER] = new FXXPMIcon(a, green_container_xpm);
    myIcons[GUIIcon::GREENEDGE] = new FXXPMIcon(a, green_edge_xpm);
    myIcons[GUIIcon::GREENVEHICLE] = new FXXPMIcon(a, green_vehicle_xpm);
    myIcons[GUIIcon::GREENPERSON] = new FXXPMIcon(a, green_person_xpm);
    myIcons[GUIIcon::YELLOWCONTAINER] = new FXXPMIcon(a, yellow_container_xpm);
    myIcons[GUIIcon::YELLOWEDGE] = new FXXPMIcon(a, yellow_edge_xpm);
    myIcons[GUIIcon::YELLOWVEHICLE] = new FXXPMIcon(a, yellow_vehicle_xpm);
    myIcons[GUIIcon::YELLOWPERSON] = new FXXPMIcon(a, yellow_person_xpm);

    myIcons[GUIIcon::COLORWHEEL] = new FXXPMIcon(a, colorwheel_xpm);
    myIcons[GUIIcon::REMOVEDB] = new FXXPMIcon(a, removedb_xpm);
    myIcons[GUIIcon::SHOWTOOLTIPS_VIEW] = new FXXPMIcon(a, show_tooltips_view_xpm);
    myIcons[GUIIcon::SHOWTOOLTIPS_MENU] = new FXXPMIcon(a, show_tooltips_menu_xpm);
    myIcons[GUIIcon::YES] = new FXXPMIcon(a, yes_xpm);
    myIcons[GUIIcon::NO] = new FXXPMIcon(a, no_xpm);
    myIcons[GUIIcon::FLAG] = new FXGIFIcon(a, flag);
    myIcons[GUIIcon::FLAG_PLUS] = new FXGIFIcon(a, flag_plus);
    myIcons[GUIIcon::FLAG_MINUS] = new FXGIFIcon(a, flag_minus);
    // window arrangements
    myIcons[GUIIcon::WINDOWS_CASCADE] = new FXXPMIcon(a, windows_cascade_xpm);
    myIcons[GUIIcon::WINDOWS_TILE_VERT] = new FXXPMIcon(a, windows_tile_vertically_xpm);
    myIcons[GUIIcon::WINDOWS_TILE_HORI] = new FXXPMIcon(a, windows_tile_horizontally_xpm);
    // manipulate
    myIcons[GUIIcon::MANIP] = new FXGIFIcon(a, manip);
    myIcons[GUIIcon::CAMERA] = new FXXPMIcon(a, camera_xpm);
    myIcons[GUIIcon::EXTRACT] = new FXXPMIcon(a, extract_streets_xpm);
    myIcons[GUIIcon::DILATE] = new FXXPMIcon(a, dilate_xpm);
    myIcons[GUIIcon::ERODE] = new FXXPMIcon(a, erode_xpm);
    myIcons[GUIIcon::OPENING] = new FXXPMIcon(a, opening_xpm);
    myIcons[GUIIcon::CLOSING] = new FXXPMIcon(a, closing_xpm);
    myIcons[GUIIcon::ERASE_STAINS] = new FXXPMIcon(a, erase_stains_xpm);
    myIcons[GUIIcon::CLOSE_GAPS] = new FXXPMIcon(a, close_gaps_xpm);
    myIcons[GUIIcon::SKELETONIZE] = new FXXPMIcon(a, skeletonize_xpm);
    myIcons[GUIIcon::RARIFY] = new FXXPMIcon(a, rarify_xpm);
    myIcons[GUIIcon::CREATE_GRAPH] = new FXXPMIcon(a, create_graph_xpm);
    myIcons[GUIIcon::EYEDROP] = new FXXPMIcon(a, eyedrop_xpm);
    myIcons[GUIIcon::PAINTBRUSH1X] = new FXXPMIcon(a, paintbrush1x_xpm);
    myIcons[GUIIcon::PAINTBRUSH2X] = new FXXPMIcon(a, paintbrush2x_xpm);
    myIcons[GUIIcon::PAINTBRUSH3X] = new FXXPMIcon(a, paintbrush3x_xpm);
    myIcons[GUIIcon::PAINTBRUSH4X] = new FXXPMIcon(a, paintbrush4x_xpm);
    myIcons[GUIIcon::PAINTBRUSH5X] = new FXXPMIcon(a, paintbrush5x_xpm);
    myIcons[GUIIcon::RUBBER1X] = new FXXPMIcon(a, rubber1x_xpm);
    myIcons[GUIIcon::RUBBER2X] = new FXXPMIcon(a, rubber2x_xpm);
    myIcons[GUIIcon::RUBBER3X] = new FXXPMIcon(a, rubber3x_xpm);
    myIcons[GUIIcon::RUBBER4X] = new FXXPMIcon(a, rubber4x_xpm);
    myIcons[GUIIcon::RUBBER5X] = new FXXPMIcon(a, rubber5x_xpm);
    myIcons[GUIIcon::EDITGRAPH] = new FXXPMIcon(a, edit_graph_xpm);

    myIcons[GUIIcon::UNDO] = new FXXPMIcon(a, undo_xpm);
    myIcons[GUIIcon::REDO] = new FXXPMIcon(a, redo_xpm);
    myIcons[GUIIcon::UNDOLIST] = new FXXPMIcon(a, undolist_xpm);
    myIcons[GUIIcon::LOCK] = new FXXPMIcon(a, lock_xpm);
    myIcons[GUIIcon::UNLOCK] = new FXXPMIcon(a, unlock_xpm);
    myIcons[GUIIcon::LOCK_SELECTED] = new FXXPMIcon(a, lockselected_xpm);
    myIcons[GUIIcon::UNLOCK_SELECTED] = new FXXPMIcon(a, unlockselected_xpm);
    myIcons[GUIIcon::ADD] = new FXXPMIcon(a, add_xpm);
    myIcons[GUIIcon::REMOVE] = new FXXPMIcon(a, remove_xpm);
    myIcons[GUIIcon::BIGARROWLEFT] = new FXXPMIcon(a, bigarrowleft_xpm);
    myIcons[GUIIcon::BIGARROWRIGHT] = new FXXPMIcon(a, bigarrowright_xpm);
    myIcons[GUIIcon::FRONTELEMENT] = new FXXPMIcon(a, frontelement_xpm);
    myIcons[GUIIcon::SIMPLIFYNETWORK] = new FXXPMIcon(a, simplifynetwork_xpm);
    myIcons[GUIIcon::COMPUTEPATHMANAGER] = new FXXPMIcon(a, computepathmanager_xpm);
    myIcons[GUIIcon::COLLAPSE] = new FXXPMIcon(a, collapse_xpm);
    myIcons[GUIIcon::UNCOLLAPSE] = new FXXPMIcon(a, uncollapse_xpm);
    myIcons[GUIIcon::EXTEND] = new FXXPMIcon(a, extend_xpm);
    myIcons[GUIIcon::CURRENT] = new FXXPMIcon(a, current_xpm);

    myIcons[GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID] = new FXXPMIcon(a, checkbox_grid_xpm);
    myIcons[GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE] = new FXXPMIcon(a, checkbox_drawjunctionshapes_xpm);
    myIcons[GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE] = new FXXPMIcon(a, checkbox_spreadvehicle_xpm);
    myIcons[GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS] = new FXXPMIcon(a, checkbox_showdemandelements_xpm);

    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_SELECTEDGES] = new FXXPMIcon(a, checkbox_selectededges_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_SHOWCONNECTIONS] = new FXXPMIcon(a, checkbox_showconnections_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_AUTOSELECTJUNCTIONS] = new FXXPMIcon(a, checkbox_autoselectjunctions_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_ASKFORMERGE] = new FXXPMIcon(a, checkbox_askformerge_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_BUBBLES] = new FXXPMIcon(a, checkbox_bubbles_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_ELEVATION] = new FXXPMIcon(a, checkbox_elevation_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_CHAIN] = new FXXPMIcon(a, checkbox_chain_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_TWOWAY] = new FXXPMIcon(a, checkbox_twoway_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_HIDECONNECTIONS] = new FXXPMIcon(a, checkbox_hideconnections_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_SHOWSUBADDITIONALS] = new FXXPMIcon(a, checkbox_showsubadditionals_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_SHOWTAZELEMENTS] = new FXXPMIcon(a, checkbox_showtazelements_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_APPLYTOALLPHASES] = new FXXPMIcon(a, checkbox_applytoallphases_xpm);

    myIcons[GUIIcon::DEMANDMODE_CHECKBOX_HIDESHAPES] = new FXXPMIcon(a, checkbox_hideshapes_xpm);
    myIcons[GUIIcon::DEMANDMODE_CHECKBOX_SHOWTRIPS] = new FXXPMIcon(a, checkbox_showtrips_xpm);
    myIcons[GUIIcon::DEMANDMODE_CHECKBOX_HIDENONINSPECTEDDEMANDELEMENTS] = new FXXPMIcon(a, checkbox_hidenoninspecteddemandelements_xpm);
    myIcons[GUIIcon::DEMANDMODE_CHECKBOX_SHOWPERSONPLANS] = new FXXPMIcon(a, checkbox_showpersonplans_xpm);
    myIcons[GUIIcon::DEMANDMODE_CHECKBOX_LOCKPERSON] = new FXXPMIcon(a, checkbox_lockperson_xpm);
    myIcons[GUIIcon::DEMANDMODE_CHECKBOX_SHOWCONTAINERPLANS] = new FXXPMIcon(a, checkbox_showcontainerplans_xpm);
    myIcons[GUIIcon::DEMANDMODE_CHECKBOX_LOCKCONTAINER] = new FXXPMIcon(a, checkbox_lockcontainer_xpm);
    myIcons[GUIIcon::DEMANDMODE_CHECKBOX_SHOWOVERLAPPEDROUTES] = new FXXPMIcon(a, checkbox_showoverlappedroutes_xpm);

    myIcons[GUIIcon::DATAMODE_CHECKBOX_SHOWADDITIONALS] = new FXXPMIcon(a, checkbox_showadditionals_xpm);
    myIcons[GUIIcon::DATAMODE_CHECKBOX_SHOWSHAPES] = new FXXPMIcon(a, checkbox_showshapes_xpm);
    myIcons[GUIIcon::DATAMODE_CHECKBOX_TAZRELDRAWING] = new FXXPMIcon(a, checkbox_tazreldrawing_xpm);
    myIcons[GUIIcon::DATAMODE_CHECKBOX_TAZDRAWFILL] = new FXXPMIcon(a, checkbox_tazdrawfill_xpm);
    myIcons[GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYFROM] = new FXXPMIcon(a, checkbox_tazrelonlyfrom_xpm);
    myIcons[GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYTO] = new FXXPMIcon(a, checkbox_tazrelonlyto_xpm);

    myIcons[GUIIcon::ARROW_UP] = new FXXPMIcon(a, arrowup_xpm);
    myIcons[GUIIcon::ARROW_DOWN] = new FXXPMIcon(a, arrowdown_xpm);
    myIcons[GUIIcon::ARROW_LEFT] = new FXXPMIcon(a, arrowleft_xpm);
    myIcons[GUIIcon::ARROW_RIGHT] = new FXXPMIcon(a, arrowright_xpm);

    myIcons[GUIIcon::LANE_PEDESTRIAN] = new FXXPMIcon(a, lanepedestrian);
    myIcons[GUIIcon::LANE_BUS] = new FXXPMIcon(a, lanebus);
    myIcons[GUIIcon::LANE_BIKE] = new FXXPMIcon(a, lanebike);
    myIcons[GUIIcon::LANEGREENVERGE] = new FXXPMIcon(a, lanegreenverge);

    myIcons[GUIIcon::EXT] = new FXXPMIcon(a, ext_xpm);
    myIcons[GUIIcon::CUT_SWELL] = new FXXPMIcon(a, cut_xpm);
    myIcons[GUIIcon::TRACKER] = new FXXPMIcon(a, tracker_xpm);

    myIcons[GUIIcon::SUPERMODENETWORK] = new FXXPMIcon(a, supermodenetwork_xpm);
    myIcons[GUIIcon::SUPERMODEDEMAND] = new FXXPMIcon(a, supermodedemand_xpm);
    myIcons[GUIIcon::SUPERMODEDATA] = new FXXPMIcon(a, supermodedata_xpm);

    myIcons[GUIIcon::MODEADDITIONAL] = new FXXPMIcon(a, modeadditional_xpm);
    myIcons[GUIIcon::MODECONNECTION] = new FXXPMIcon(a, modeconnection_xpm);
    myIcons[GUIIcon::MODECREATEEDGE] = new FXXPMIcon(a, modecreateedge_xpm);
    myIcons[GUIIcon::MODECROSSING] = new FXXPMIcon(a, modecrossing_xpm);
    myIcons[GUIIcon::MODETAZ] = new FXXPMIcon(a, modetaz_xpm);
    myIcons[GUIIcon::MODEDELETE] = new FXXPMIcon(a, modedelete_xpm);
    myIcons[GUIIcon::MODEINSPECT] = new FXXPMIcon(a, modeinspect_xpm);
    myIcons[GUIIcon::MODEMOVE] = new FXXPMIcon(a, modemove_xpm);
    myIcons[GUIIcon::MODESELECT] = new FXXPMIcon(a, modeselect_xpm);
    myIcons[GUIIcon::MODETLS] = new FXXPMIcon(a, modetrafficlight_xpm);
    myIcons[GUIIcon::MODESHAPE] = new FXXPMIcon(a, modeshape_xpm);
    myIcons[GUIIcon::MODEPROHIBITION] = new FXXPMIcon(a, modeprohibition_xpm);
    myIcons[GUIIcon::MODEWIRE] = new FXXPMIcon(a, modewire_xpm);

    myIcons[GUIIcon::MODEROUTE] = new FXXPMIcon(a, moderoute_xpm);
    myIcons[GUIIcon::MODEVEHICLE] = new FXXPMIcon(a, modevehicle_xpm);
    myIcons[GUIIcon::MODETYPE] = new FXXPMIcon(a, modetype_xpm);
    myIcons[GUIIcon::MODESTOP] = new FXXPMIcon(a, modestop_xpm);
    myIcons[GUIIcon::MODEPERSON] = new FXXPMIcon(a, modeperson_xpm);
    myIcons[GUIIcon::MODEPERSONPLAN] = new FXXPMIcon(a, modepersonplan_xpm);
    myIcons[GUIIcon::MODECONTAINER] = new FXXPMIcon(a, modecontainer_xpm);
    myIcons[GUIIcon::MODECONTAINERPLAN] = new FXXPMIcon(a, modecontainerplan_xpm);

    myIcons[GUIIcon::MODEEDGEDATA] = new FXXPMIcon(a, modeedgedata_xpm);
    myIcons[GUIIcon::MODEEDGERELDATA] = new FXXPMIcon(a, modeedgereldata_xpm);
    myIcons[GUIIcon::MODETAZRELDATA] = new FXXPMIcon(a, modetazreldata_xpm);
    myIcons[GUIIcon::MODEMEANDATA] = new FXXPMIcon(a, modemeandata_xpm);
    myIcons[GUIIcon::MEANDATAEDGE] = new FXXPMIcon(a, meandataedge_xpm);
    myIcons[GUIIcon::MEANDATALANE] = new FXXPMIcon(a, meandatalane_xpm);

    myIcons[GUIIcon::COMPUTEJUNCTIONS] = new FXXPMIcon(a, computejunctions_xpm);
    myIcons[GUIIcon::CLEANJUNCTIONS] = new FXXPMIcon(a, cleanjunctions_xpm);
    myIcons[GUIIcon::JOINJUNCTIONS] = new FXXPMIcon(a, joinjunctions_xpm);
    myIcons[GUIIcon::COMPUTEDEMAND] = new FXXPMIcon(a, computedemand_xpm);
    myIcons[GUIIcon::CLEANROUTES] = new FXXPMIcon(a, cleanroutes_xpm);
    myIcons[GUIIcon::JOINROUTES] = new FXXPMIcon(a, joinroutes_xpm);
    myIcons[GUIIcon::ADJUSTPERSONPLANS] = new FXXPMIcon(a, adjustpersonplans_xpm);

    myIcons[GUIIcon::OPTIONS] = new FXXPMIcon(a, options_xpm);

    myIcons[GUIIcon::JUNCTION] = new FXXPMIcon(a, junction_xpm);
    myIcons[GUIIcon::EDGETYPE] = new FXXPMIcon(a, edgetype_xpm);
    myIcons[GUIIcon::LANETYPE] = new FXXPMIcon(a, lanetype_xpm);
    myIcons[GUIIcon::EDGE] = new FXXPMIcon(a, edge_xpm);
    myIcons[GUIIcon::LANE] = new FXXPMIcon(a, lane_xpm);
    myIcons[GUIIcon::CONNECTION] = new FXXPMIcon(a, connection_xpm);
    myIcons[GUIIcon::PROHIBITION] = new FXXPMIcon(a, prohibition_xpm);
    myIcons[GUIIcon::CROSSING] = new FXXPMIcon(a, crossing_xpm);
    myIcons[GUIIcon::WALKINGAREA] = new FXXPMIcon(a, walkingarea_xpm);

    myIcons[GUIIcon::BUSSTOP] = new FXXPMIcon(a, busstop_xpm);
    myIcons[GUIIcon::TRAINSTOP] = new FXXPMIcon(a, trainstop_xpm);
    myIcons[GUIIcon::ACCESS] = new FXXPMIcon(a, access_xpm);
    myIcons[GUIIcon::CONTAINERSTOP] = new FXXPMIcon(a, containerstop_xpm);
    myIcons[GUIIcon::CHARGINGSTATION] = new FXXPMIcon(a, chargingstation_xpm);
    myIcons[GUIIcon::PARKINGAREA] = new FXXPMIcon(a, parkingarea_xpm);
    myIcons[GUIIcon::PARKINGSPACE] = new FXXPMIcon(a, parkingspace_xpm);
    myIcons[GUIIcon::E1] = new FXXPMIcon(a, e1_xpm);
    myIcons[GUIIcon::E2] = new FXXPMIcon(a, e2_xpm);
    myIcons[GUIIcon::E3] = new FXXPMIcon(a, e3_xpm);
    myIcons[GUIIcon::E3ENTRY] = new FXXPMIcon(a, e3entry_xpm);
    myIcons[GUIIcon::E3EXIT] = new FXXPMIcon(a, e3exit_xpm);
    myIcons[GUIIcon::E1INSTANT] = new FXXPMIcon(a, e1instant_xpm);
    myIcons[GUIIcon::REROUTER] = new FXXPMIcon(a, rerouter_xpm);
    myIcons[GUIIcon::ROUTEPROBE] = new FXXPMIcon(a, routeprobe_xpm);
    myIcons[GUIIcon::VAPORIZER] = new FXXPMIcon(a, vaporizer_xpm);
    myIcons[GUIIcon::VARIABLESPEEDSIGN] = new FXXPMIcon(a, variablespeedsign_xpm);
    myIcons[GUIIcon::CALIBRATOR] = new FXXPMIcon(a, calibrator_xpm);
    myIcons[GUIIcon::REROUTERINTERVAL] = new FXXPMIcon(a, rerouterinterval_xpm);
    myIcons[GUIIcon::VSSSTEP] = new FXXPMIcon(a, vssstep_xpm);
    myIcons[GUIIcon::CLOSINGREROUTE] = new FXXPMIcon(a, closingreroute_xpm);
    myIcons[GUIIcon::CLOSINGLANEREROUTE] = new FXXPMIcon(a, closinglanereroute_xpm);
    myIcons[GUIIcon::DESTPROBREROUTE] = new FXXPMIcon(a, destprobreroute_xpm);
    myIcons[GUIIcon::PARKINGZONEREROUTE] = new FXXPMIcon(a, parkingzonereroute_xpm);
    myIcons[GUIIcon::ROUTEPROBREROUTE] = new FXXPMIcon(a, routeprobreroute_xpm);

    myIcons[GUIIcon::TRACTION_SUBSTATION] = new FXXPMIcon(a, tractionsubstation_xpm);
    myIcons[GUIIcon::OVERHEADWIRE] = new FXXPMIcon(a, overheadwire_xpm);
    myIcons[GUIIcon::OVERHEADWIRE_CLAMP] = new FXXPMIcon(a, overheadwire_clamp_xpm);

    myIcons[GUIIcon::POLY] = new FXXPMIcon(a, poly_xpm);
    myIcons[GUIIcon::POI] = new FXXPMIcon(a, poi_xpm);
    myIcons[GUIIcon::POILANE] = new FXXPMIcon(a, poilane_xpm);
    myIcons[GUIIcon::POIGEO] = new FXXPMIcon(a, poigeo_xpm);

    myIcons[GUIIcon::TAZ] = new FXXPMIcon(a, taz_xpm);
    myIcons[GUIIcon::TAZEDGE] = new FXXPMIcon(a, tazedge_xpm);

    myIcons[GUIIcon::ROUTE] = new FXXPMIcon(a, route_xpm);
    myIcons[GUIIcon::VTYPE] = new FXXPMIcon(a, vtype_xpm);
    myIcons[GUIIcon::VTYPEDISTRIBUTION] = new FXXPMIcon(a, vtypedistribution_xpm);
    myIcons[GUIIcon::VEHICLE] = new FXXPMIcon(a, vehicle_xpm);
    myIcons[GUIIcon::TRIP] = new FXXPMIcon(a, trip_xpm);
    myIcons[GUIIcon::TRIP_JUNCTIONS] = new FXXPMIcon(a, tripjunctions_xpm);
    myIcons[GUIIcon::FLOW] = new FXXPMIcon(a, flow_xpm);
    myIcons[GUIIcon::FLOW_JUNCTIONS] = new FXXPMIcon(a, flowjunctions_xpm);
    myIcons[GUIIcon::ROUTEFLOW] = new FXXPMIcon(a, routeflow_xpm);
    myIcons[GUIIcon::STOPELEMENT] = new FXXPMIcon(a, stopelement_xpm);
    myIcons[GUIIcon::WAYPOINT] = new FXXPMIcon(a, waypoint_xpm);
    myIcons[GUIIcon::PERSON] = new FXXPMIcon(a, person_xpm);
    myIcons[GUIIcon::PERSONFLOW] = new FXXPMIcon(a, personflow_xpm);
    myIcons[GUIIcon::PERSONTRIP_FROMTO] = new FXXPMIcon(a, persontripfromto_xpm);
    myIcons[GUIIcon::PERSONTRIP_BUSSTOP] = new FXXPMIcon(a, persontripbusstop_xpm);
    myIcons[GUIIcon::PERSONTRIP_JUNCTIONS] = new FXXPMIcon(a, persontripjunctions_xpm);
    myIcons[GUIIcon::WALK_EDGES] = new FXXPMIcon(a, walkedges_xpm);
    myIcons[GUIIcon::WALK_FROMTO] = new FXXPMIcon(a, walkfromto_xpm);
    myIcons[GUIIcon::WALK_BUSSTOP] = new FXXPMIcon(a, walkbusstop_xpm);
    myIcons[GUIIcon::WALK_ROUTE] = new FXXPMIcon(a, walkroute_xpm);
    myIcons[GUIIcon::WALK_JUNCTIONS] = new FXXPMIcon(a, walkjunctions_xpm);
    myIcons[GUIIcon::RIDE_FROMTO] = new FXXPMIcon(a, ridefromto_xpm);
    myIcons[GUIIcon::RIDE_BUSSTOP] = new FXXPMIcon(a, ridebusstop_xpm);

    myIcons[GUIIcon::CONTAINER] = new FXXPMIcon(a, container_xpm);
    myIcons[GUIIcon::CONTAINERFLOW] = new FXXPMIcon(a, containerflow_xpm);
    myIcons[GUIIcon::TRANSPORT_FROMTO] = new FXXPMIcon(a, transportfromto_xpm);
    myIcons[GUIIcon::TRANSPORT_CONTAINERSTOP] = new FXXPMIcon(a, transportcontainerstop_xpm);
    myIcons[GUIIcon::TRANSHIP_EDGES] = new FXXPMIcon(a, transhipedges_xpm);
    myIcons[GUIIcon::TRANSHIP_FROMTO] = new FXXPMIcon(a, transhipfromto_xpm);
    myIcons[GUIIcon::TRANSHIP_CONTAINERSTOP] = new FXXPMIcon(a, transhipcontainerstop_xpm);

    myIcons[GUIIcon::DATASET] = new FXXPMIcon(a, dataset_xpm);
    myIcons[GUIIcon::DATAINTERVAL] = new FXXPMIcon(a, datainterval_xpm);
    myIcons[GUIIcon::EDGEDATA] = new FXXPMIcon(a, edgedata_xpm);
    myIcons[GUIIcon::EDGERELDATA] = new FXXPMIcon(a, edgereldata_xpm);
    myIcons[GUIIcon::TAZRELDATA] = new FXXPMIcon(a, tazreldata_xpm);

    myIcons[GUIIcon::TLSPHASEDEFAULT] = new FXXPMIcon(a, tlsphasedefault_xpm);
    myIcons[GUIIcon::TLSPHASECOPY] = new FXXPMIcon(a, tlsphasecopy_xpm);
    myIcons[GUIIcon::TLSPHASEALLRED] = new FXXPMIcon(a, tlsphaseallred_xpm);
    myIcons[GUIIcon::TLSPHASEALLYELLOW] = new FXXPMIcon(a, tlsphaseallyellow_xpm);
    myIcons[GUIIcon::TLSPHASEALLGREEN] = new FXXPMIcon(a, tlsphaseallgreen_xpm);
    myIcons[GUIIcon::TLSPHASEALLGREENPRIORITY] = new FXXPMIcon(a, tlsphaseallgreenpriority_xpm);

    myIcons[GUIIcon::VCLASS_IGNORING] = new FXXPMIcon(a, vclass_ignoring_xpm);
    myIcons[GUIIcon::VCLASS_PRIVATE] = new FXXPMIcon(a, vclass_private_xpm);
    myIcons[GUIIcon::VCLASS_EMERGENCY] = new FXXPMIcon(a, vclass_emergency_xpm);
    myIcons[GUIIcon::VCLASS_AUTHORITY] = new FXXPMIcon(a, vclass_authority_xpm);
    myIcons[GUIIcon::VCLASS_ARMY] = new FXXPMIcon(a, vclass_army_xpm);
    myIcons[GUIIcon::VCLASS_VIP] = new FXXPMIcon(a, vclass_vip_xpm);
    myIcons[GUIIcon::VCLASS_PASSENGER] = new FXXPMIcon(a, vclass_passenger_xpm);
    myIcons[GUIIcon::VCLASS_HOV] = new FXXPMIcon(a, vclass_hov_xpm);
    myIcons[GUIIcon::VCLASS_TAXI] = new FXXPMIcon(a, vclass_taxi_xpm);
    myIcons[GUIIcon::VCLASS_BUS] = new FXXPMIcon(a, vclass_bus_xpm);
    myIcons[GUIIcon::VCLASS_COACH] = new FXXPMIcon(a, vclass_coach_xpm);
    myIcons[GUIIcon::VCLASS_DELIVERY] = new FXXPMIcon(a, vclass_delivery_xpm);
    myIcons[GUIIcon::VCLASS_TRUCK] = new FXXPMIcon(a, vclass_truck_xpm);
    myIcons[GUIIcon::VCLASS_TRAILER] = new FXXPMIcon(a, vclass_trailer_xpm);
    myIcons[GUIIcon::VCLASS_TRAM] = new FXXPMIcon(a, vclass_tram_xpm);
    myIcons[GUIIcon::VCLASS_RAIL_URBAN] = new FXXPMIcon(a, vclass_rail_urban_xpm);
    myIcons[GUIIcon::VCLASS_RAIL] = new FXXPMIcon(a, vclass_rail_xpm);
    myIcons[GUIIcon::VCLASS_RAIL_ELECTRIC] = new FXXPMIcon(a, vclass_rail_electric_xpm);
    myIcons[GUIIcon::VCLASS_RAIL_FAST] = new FXXPMIcon(a, vclass_rail_fast_xpm);
    myIcons[GUIIcon::VCLASS_MOTORCYCLE] = new FXXPMIcon(a, vclass_motorcycle_xpm);
    myIcons[GUIIcon::VCLASS_MOPED] = new FXXPMIcon(a, vclass_moped_xpm);
    myIcons[GUIIcon::VCLASS_BICYCLE] = new FXXPMIcon(a, vclass_bicycle_xpm);
    myIcons[GUIIcon::VCLASS_PEDESTRIAN] = new FXXPMIcon(a, vclass_pedestrian_xpm);
    myIcons[GUIIcon::VCLASS_EVEHICLE] = new FXXPMIcon(a, vclass_evehicle_xpm);
    myIcons[GUIIcon::VCLASS_SHIP] = new FXXPMIcon(a, vclass_ship_xpm);
    myIcons[GUIIcon::VCLASS_CUSTOM1] = new FXXPMIcon(a, vclass_custom1_xpm);
    myIcons[GUIIcon::VCLASS_CUSTOM2] = new FXXPMIcon(a, vclass_custom2_xpm);

    myIcons[GUIIcon::VCLASS_SMALL_IGNORING] = new FXXPMIcon(a, vclass_small_ignoring_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_PRIVATE] = new FXXPMIcon(a, vclass_small_private_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_EMERGENCY] = new FXXPMIcon(a, vclass_small_emergency_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_AUTHORITY] = new FXXPMIcon(a, vclass_small_authority_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_ARMY] = new FXXPMIcon(a, vclass_small_army_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_VIP] = new FXXPMIcon(a, vclass_small_vip_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_PASSENGER] = new FXXPMIcon(a, vclass_small_passenger_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_HOV] = new FXXPMIcon(a, vclass_small_hov_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_TAXI] = new FXXPMIcon(a, vclass_small_taxi_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_BUS] = new FXXPMIcon(a, vclass_small_bus_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_COACH] = new FXXPMIcon(a, vclass_small_coach_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_DELIVERY] = new FXXPMIcon(a, vclass_small_delivery_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_TRUCK] = new FXXPMIcon(a, vclass_small_truck_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_TRAILER] = new FXXPMIcon(a, vclass_small_trailer_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_TRAM] = new FXXPMIcon(a, vclass_small_tram_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_RAIL_URBAN] = new FXXPMIcon(a, vclass_small_rail_urban_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_RAIL] = new FXXPMIcon(a, vclass_small_rail_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_RAIL_ELECTRIC] = new FXXPMIcon(a, vclass_small_rail_electric_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_RAIL_FAST] = new FXXPMIcon(a, vclass_small_rail_fast_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_MOTORCYCLE] = new FXXPMIcon(a, vclass_small_motorcycle_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_MOPED] = new FXXPMIcon(a, vclass_small_moped_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_BICYCLE] = new FXXPMIcon(a, vclass_small_bicycle_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_PEDESTRIAN] = new FXXPMIcon(a, vclass_small_pedestrian_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_EVEHICLE] = new FXXPMIcon(a, vclass_small_evehicle_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_SHIP] = new FXXPMIcon(a, vclass_small_ship_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_CUSTOM1] = new FXXPMIcon(a, vclass_small_custom1_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_CUSTOM2] = new FXXPMIcon(a, vclass_small_custom2_xpm);

    myIcons[GUIIcon::VSHAPE_PEDESTRIAN] = new FXXPMIcon(a, vshape_pedestrian_xpm);
    myIcons[GUIIcon::VSHAPE_BICYCLE] = new FXXPMIcon(a, vshape_bicycle_xpm);
    myIcons[GUIIcon::VSHAPE_MOPED] = new FXXPMIcon(a, vshape_moped_xpm);
    myIcons[GUIIcon::VSHAPE_MOTORCYCLE] = new FXXPMIcon(a, vshape_motorcycle_xpm);
    myIcons[GUIIcon::VSHAPE_PASSENGER] = new FXXPMIcon(a, vshape_passenger_xpm);
    myIcons[GUIIcon::VSHAPE_PASSENGER_SEDAN] = new FXXPMIcon(a, vshape_passenger_sedan_xpm);
    myIcons[GUIIcon::VSHAPE_PASSENGER_HATCHBACK] = new FXXPMIcon(a, vshape_passenger_hatchback_xpm);
    myIcons[GUIIcon::VSHAPE_PASSENGER_WAGON] = new FXXPMIcon(a, vshape_passenger_wagon_xpm);
    myIcons[GUIIcon::VSHAPE_PASSENGER_VAN] = new FXXPMIcon(a, vshape_passenger_van_xpm);
    myIcons[GUIIcon::VSHAPE_TAXI] = new FXXPMIcon(a, vshape_taxi_xpm);
    myIcons[GUIIcon::VSHAPE_DELIVERY] = new FXXPMIcon(a, vshape_delivery_xpm);
    myIcons[GUIIcon::VSHAPE_TRUCK] = new FXXPMIcon(a, vshape_truck_xpm);
    myIcons[GUIIcon::VSHAPE_TRUCK_SEMITRAILER] = new FXXPMIcon(a, vshape_truck_semitrailer_xpm);
    myIcons[GUIIcon::VSHAPE_TRUCK_1TRAILER] = new FXXPMIcon(a, vshape_truck_1trailer_xpm);
    myIcons[GUIIcon::VSHAPE_BUS] = new FXXPMIcon(a, vshape_bus_xpm);
    myIcons[GUIIcon::VSHAPE_BUS_COACH] = new FXXPMIcon(a, vshape_bus_coach_xpm);
    myIcons[GUIIcon::VSHAPE_BUS_FLEXIBLE] = new FXXPMIcon(a, vshape_bus_flexible_xpm);
    myIcons[GUIIcon::VSHAPE_BUS_TROLLEY] = new FXXPMIcon(a, vshape_bus_trolley_xpm);
    myIcons[GUIIcon::VSHAPE_RAIL] = new FXXPMIcon(a, vshape_rail_xpm);
    myIcons[GUIIcon::VSHAPE_RAIL_CAR] = new FXXPMIcon(a, vshape_rail_car_xpm);
    myIcons[GUIIcon::VSHAPE_RAIL_CARGO] = new FXXPMIcon(a, vshape_rail_cargo_xpm);
    myIcons[GUIIcon::VSHAPE_E_VEHICLE] = new FXXPMIcon(a, vshape_e_vehicle_xpm);
    myIcons[GUIIcon::VSHAPE_ANT] = new FXXPMIcon(a, vshape_ant_xpm);
    myIcons[GUIIcon::VSHAPE_SHIP] = new FXXPMIcon(a, vshape_ship_xpm);
    myIcons[GUIIcon::VSHAPE_EMERGENCY] = new FXXPMIcon(a, vshape_emergency_xpm);
    myIcons[GUIIcon::VSHAPE_FIREBRIGADE] = new FXXPMIcon(a, vshape_firebrigade_xpm);
    myIcons[GUIIcon::VSHAPE_POLICE] = new FXXPMIcon(a, vshape_police_xpm);
    myIcons[GUIIcon::VSHAPE_RICKSHAW] = new FXXPMIcon(a, vshape_rickshaw_xpm);
    myIcons[GUIIcon::VSHAPE_AIRCRAFT] = new FXXPMIcon(a, vshape_aircraft_xpm);
    myIcons[GUIIcon::VSHAPE_SCOOTER] = new FXXPMIcon(a, vshape_scooter_xpm);
    myIcons[GUIIcon::VSHAPE_UNKNOWN] = new FXXPMIcon(a, vshape_unknown_xpm);

    myIcons[GUIIcon::OK] = new FXXPMIcon(a, accept_xpm);
    myIcons[GUIIcon::ACCEPT] = new FXXPMIcon(a, accept_xpm);
    myIcons[GUIIcon::CANCEL] = new FXXPMIcon(a, cancel_xpm);
    myIcons[GUIIcon::CORRECT] = new FXXPMIcon(a, correct_xpm);
    myIcons[GUIIcon::INCORRECT] = new FXXPMIcon(a, incorrect_xpm);
    myIcons[GUIIcon::RESET] = new FXXPMIcon(a, reset_xpm);
    myIcons[GUIIcon::WARNING] = new FXXPMIcon(a, warning_xpm);
    myIcons[GUIIcon::DANGER] = new FXXPMIcon(a, danger_xpm);
    myIcons[GUIIcon::SUM] = new FXXPMIcon(a, sum_xpm);
    myIcons[GUIIcon::BACK] = new FXXPMIcon(a, back_xpm);

    myIcons[GUIIcon::GRID] = new FXXPMIcon(a, grid_xpm);
    myIcons[GUIIcon::GRID1] = new FXXPMIcon(a, grid1_xpm);
    myIcons[GUIIcon::GRID2] = new FXXPMIcon(a, grid2_xpm);
    myIcons[GUIIcon::GRID3] = new FXXPMIcon(a, grid3_xpm);

    myIcons[GUIIcon::GEOHACK] = new FXXPMIcon(a, geohack_xpm);
    myIcons[GUIIcon::GOOGLESAT] = new FXXPMIcon(a, googlesat_xpm);
    myIcons[GUIIcon::OSM] = new FXXPMIcon(a, osm_xpm);

    myIcons[GUIIcon::LANGUAGE_EN] = new FXXPMIcon(a, language_en_xpm);
    myIcons[GUIIcon::LANGUAGE_DE] = new FXXPMIcon(a, language_de_xpm);
    myIcons[GUIIcon::LANGUAGE_ES] = new FXXPMIcon(a, language_es_xpm);
    myIcons[GUIIcon::LANGUAGE_FR] = new FXXPMIcon(a, language_fr_xpm);
    myIcons[GUIIcon::LANGUAGE_IT] = new FXXPMIcon(a, language_it_xpm);
    myIcons[GUIIcon::LANGUAGE_HU] = new FXXPMIcon(a, language_hu_xpm);
    myIcons[GUIIcon::LANGUAGE_TR] = new FXXPMIcon(a, language_tr_xpm);
    myIcons[GUIIcon::LANGUAGE_ZH] = new FXXPMIcon(a, language_zh_xpm);
    myIcons[GUIIcon::LANGUAGE_ZHT] = new FXXPMIcon(a, language_zht_xpm);

    // ... and create them
    for (const auto& icon : myIcons) {
        icon.second->create();
    }
}


GUIIconSubSys::~GUIIconSubSys() {
    // remove all icons
    for (const auto& icon : myIcons) {
        delete icon.second;
    }
}


void
GUIIconSubSys::initIcons(FXApp* a) {
    if (myInstance) {
        throw ProcessError(TL("Instance was previously created"));
    } else {
        myInstance = new GUIIconSubSys(a);
    }
}


FXIcon*
GUIIconSubSys::getIcon(const GUIIcon which) {
    return myInstance->myIcons.at(which);
}


void
GUIIconSubSys::close() {
    // delete instance and set null
    delete myInstance;
    myInstance = nullptr;
}


/****************************************************************************/
