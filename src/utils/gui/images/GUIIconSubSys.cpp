/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include "icons/sumo_icon64.xpm"
#include "icons/sumo_icon16.xpm"
#include "icons/sumo_logo.xpm"

#include "icons/netgenerate.xpm"
#include "icons/netedit_icon64.xpm"
#include "icons/netedit_icon16.xpm"

#include "icons/netgenerate_grid.xpm"
#include "icons/netgenerate_spider.xpm"
#include "icons/netgenerate_randomgrid.xpm"
#include "icons/netgenerate_random.xpm"

#include "icons/empty.xpm"
#include "icons/newnet.xpm"
#include "icons/play.xpm"
#include "icons/stop.xpm"
#include "icons/step.xpm"
#include "icons/new_window.xpm"
#include "icons/new_window_3d.xpm"

#include "icons/open.xpm"
#include "icons/openadditionals.xpm"
#include "icons/openbmpdialog.xpm"
#include "icons/opennet.xpm"
#include "icons/openmeandatas.xpm"
#include "icons/openneteditconfig.xpm"
#include "icons/openshapes.xpm"
#include "icons/opensumoconfig.xpm"
#include "icons/opentls.xpm"

#include "icons/save.xpm"
#include "icons/save_sumoconfig.xpm"
#include "icons/save_neteditconfig.xpm"
#include "icons/save_multiple.xpm"
#include "icons/save_networkelements.xpm"
#include "icons/save_aditionalelements.xpm"
#include "icons/save_jupedsimelements.xpm"
#include "icons/save_demandelements.xpm"
#include "icons/save_dataelements.xpm"
#include "icons/save_meandataelements.xpm"
#include "icons/save_database.xpm"

#include "icons/app_tracker.xpm"
#include "icons/app_finder.xpm"
#include "icons/app_breakpoints.xpm"
#include "icons/app_tlstracker.xpm"
#include "icons/app_table.xpm"
#include "icons/app_selector.xpm"

#include "icons/locate.xpm"
#include "icons/locate_junction.xpm"
#include "icons/locate_edge.xpm"
#include "icons/locate_walkingarea.xpm"
#include "icons/locate_vehicle.xpm"
#include "icons/locate_route.xpm"
#include "icons/locate_stop.xpm"
#include "icons/locate_person.xpm"
#include "icons/locate_container.xpm"
#include "icons/locate_tls.xpm"
#include "icons/locate_add.xpm"
#include "icons/locate_poi.xpm"
#include "icons/locate_poly.xpm"

#include "icons/tool_python.xpm"
#include "icons/tool_emissions.xpm"
#include "icons/tool_citybrain.xpm"
#include "icons/tool_gtfs.xpm"
#include "icons/tool_vissim.xpm"
#include "icons/tool_visum.xpm"
#include "icons/tool_import.xpm"
#include "icons/tool_output.xpm"
#include "icons/tool_turndefs.xpm"
#include "icons/tool_xml.xpm"

#include "icons/green_container.xpm"
#include "icons/yellow_container.xpm"
#include "icons/green_edge.xpm"
#include "icons/yellow_edge.xpm"
#include "icons/green_vehicle.xpm"
#include "icons/yellow_vehicle.xpm"
#include "icons/green_person.xpm"
#include "icons/yellow_person.xpm"

#include "icons/colorwheel.xpm"
#include "icons/removedb.xpm"
#include "icons/recenter_view.xpm"
#include "icons/allow_rotation.xpm"
#include "icons/show_tooltips_view.xpm"
#include "icons/show_tooltips_menu.xpm"
#include "icons/magnify.xpm"
#include "icons/zoomstyle.xpm"
#include "icons/yes.xpm"
#include "icons/no.xpm"

#include "icons/extract_streets.xpm"
#include "icons/dilate.xpm"
#include "icons/erode.xpm"
#include "icons/opening.xpm"
#include "icons/closing.xpm"
#include "icons/erase_stains.xpm"
#include "icons/close_gaps.xpm"
#include "icons/skeletonize.xpm"
#include "icons/rarify.xpm"
#include "icons/create_graph.xpm"
#include "icons/eyedrop.xpm"
#include "icons/rubber1x.xpm"
#include "icons/rubber2x.xpm"
#include "icons/rubber3x.xpm"
#include "icons/rubber4x.xpm"
#include "icons/rubber5x.xpm"
#include "icons/paintbrush1x.xpm"
#include "icons/paintbrush2x.xpm"
#include "icons/paintbrush3x.xpm"
#include "icons/paintbrush4x.xpm"
#include "icons/paintbrush5x.xpm"
#include "icons/edit_graph.xpm"
#include "icons/full_screen.xpm"

#include "icons/flag.xpm"
#include "icons/flag_plus.xpm"
#include "icons/flag_minus.xpm"
#include "icons/windows_cascade.xpm"
#include "icons/windows_tile_vertically.xpm"
#include "icons/windows_tile_horizontally.xpm"
#include "icons/reload.xpm"
#include "icons/manip.xpm"
#include "icons/camera.xpm"

#include "icons/undo.xpm"
#include "icons/redo.xpm"
#include "icons/undolist.xpm"
#include "icons/lock.xpm"
#include "icons/unlock.xpm"
#include "icons/lock_selected.xpm"
#include "icons/unlock_selected.xpm"
#include "icons/add.xpm"
#include "icons/remove.xpm"
#include "icons/collapse.xpm"
#include "icons/uncollapse.xpm"
#include "icons/extend.xpm"
#include "icons/current.xpm"
#include "icons/search.xpm"
#include "icons/allow.xpm"

#include "icons/arrowup.xpm"
#include "icons/arrowdown.xpm"
#include "icons/arrowleft.xpm"
#include "icons/arrowright.xpm"

#include "icons/bigarrowleft.xpm"
#include "icons/bigarrowright.xpm"

#include "icons/frontelement.xpm"
#include "icons/simplifynetwork.xpm"
#include "icons/computepathmanager.xpm"

#include "icons/checkboxes/checkbox_applytoallphases.xpm"
#include "icons/checkboxes/checkbox_mergeautomatically.xpm"
#include "icons/checkboxes/checkbox_autoselectjunctions.xpm"
#include "icons/checkboxes/checkbox_bubbles.xpm"
#include "icons/checkboxes/checkbox_chain.xpm"
#include "icons/checkboxes/checkbox_elevation.xpm"
#include "icons/checkboxes/checkbox_grid.xpm"
#include "icons/checkboxes/checkbox_drawjunctionshapes.xpm"
#include "icons/checkboxes/checkbox_hideconnections.xpm"
#include "icons/checkboxes/checkbox_showsubadditionals.xpm"
#include "icons/checkboxes/checkbox_showtazelements.xpm"
#include "icons/checkboxes/checkbox_hidenoninspecteddemandelements.xpm"
#include "icons/checkboxes/checkbox_hideshapes.xpm"
#include "icons/checkboxes/checkbox_lockcontainer.xpm"
#include "icons/checkboxes/checkbox_lockperson.xpm"
#include "icons/checkboxes/checkbox_selectededges.xpm"
#include "icons/checkboxes/checkbox_showadditionals.xpm"
#include "icons/checkboxes/checkbox_showconnections.xpm"
#include "icons/checkboxes/checkbox_showcontainerplans.xpm"
#include "icons/checkboxes/checkbox_showdemandelements.xpm"
#include "icons/checkboxes/checkbox_showoverlappedroutes.xpm"
#include "icons/checkboxes/checkbox_showpersonplans.xpm"
#include "icons/checkboxes/checkbox_showshapes.xpm"
#include "icons/checkboxes/checkbox_showtrips.xpm"
#include "icons/checkboxes/checkbox_spreadvehicle.xpm"
#include "icons/checkboxes/checkbox_tazdrawfill.xpm"
#include "icons/checkboxes/checkbox_tazreldrawing.xpm"
#include "icons/checkboxes/checkbox_tazrelonlyfrom.xpm"
#include "icons/checkboxes/checkbox_tazrelonlyto.xpm"
#include "icons/checkboxes/checkbox_twoway.xpm"

#include "icons/lanebike.xpm"
#include "icons/lanebus.xpm"
#include "icons/lanepedestrian.xpm"
#include "icons/lanegreenverge.xpm"

#include "icons/cut.xpm"
#include "icons/copy.xpm"
#include "icons/paste.xpm"

#include "icons/select.xpm"
#include "icons/unselect.xpm"

#include "icons/join.xpm"
#include "icons/disjoin.xpm"

#include "icons/ext.xpm"
#include "icons/tracker.xpm"
#include "icons/hall_of_fame.xpm"
#include "icons/clear_message_window.xpm"
#include "icons/github.xpm"

#include "icons/viewdefault.xpm"
#include "icons/viewjupedsim.xpm"

#include "icons/supermodenetwork.xpm"
#include "icons/supermodedemand.xpm"
#include "icons/supermodedata.xpm"

#include "icons/modeadditional.xpm"
#include "icons/modeconnection.xpm"
#include "icons/modecreateedge.xpm"
#include "icons/modecrossing.xpm"
#include "icons/modetaz.xpm"
#include "icons/modedelete.xpm"
#include "icons/modeinspect.xpm"
#include "icons/modemove.xpm"
#include "icons/modeselect.xpm"
#include "icons/modetrafficlight.xpm"
#include "icons/modeshape.xpm"
#include "icons/modeprohibition.xpm"
#include "icons/modewire.xpm"
#include "icons/modedecal.xpm"

#include "icons/modecontainer.xpm"
#include "icons/modecontainerplan.xpm"
#include "icons/modeperson.xpm"
#include "icons/modepersonplan.xpm"
#include "icons/moderoute.xpm"
#include "icons/moderoutedistribution.xpm"
#include "icons/modestop.xpm"
#include "icons/modevehicle.xpm"
#include "icons/modetype.xpm"
#include "icons/modetypedistribution.xpm"

#include "icons/modeedgedata.xpm"
#include "icons/modeedgereldata.xpm"
#include "icons/modetazreldata.xpm"
#include "icons/modemeandata.xpm"
#include "icons/meandataedge.xpm"
#include "icons/meandatalane.xpm"

#include "icons/tlsphasedefault.xpm"
#include "icons/tlsphasecopy.xpm"
#include "icons/tlsphaseallred.xpm"
#include "icons/tlsphaseallyellow.xpm"
#include "icons/tlsphaseallgreen.xpm"
#include "icons/tlsphaseallgreenpriority.xpm"

#include "icons/computejunctions.xpm"
#include "icons/cleanjunctions.xpm"
#include "icons/joinjunctions.xpm"
#include "icons/computedemand.xpm"
#include "icons/cleanroutes.xpm"
#include "icons/joinroutes.xpm"
#include "icons/adjustpersonplans.xpm"
#include "icons/options.xpm"

#include "icons/junction.xpm"
#include "icons/edgetype.xpm"
#include "icons/lanetype.xpm"
#include "icons/edge.xpm"
#include "icons/lane.xpm"
#include "icons/connection.xpm"
#include "icons/prohibition.xpm"
#include "icons/crossing.xpm"
#include "icons/walkingarea.xpm"

#include "icons/busstop.xpm"
#include "icons/trainstop.xpm"
#include "icons/access.xpm"
#include "icons/chargingstation.xpm"
#include "icons/containerstop.xpm"
#include "icons/parkingarea.xpm"
#include "icons/parkingspace.xpm"
#include "icons/e1.xpm"
#include "icons/e1instant.xpm"
#include "icons/e2.xpm"
#include "icons/e3.xpm"
#include "icons/e3entry.xpm"
#include "icons/e3exit.xpm"
#include "icons/rerouter.xpm"
#include "icons/routeprobe.xpm"
#include "icons/vaporizer.xpm"
#include "icons/variablespeedsign.xpm"
#include "icons/calibrator.xpm"
#include "icons/rerouterinterval.xpm"
#include "icons/vssstep.xpm"
#include "icons/closingreroute.xpm"
#include "icons/closinglanereroute.xpm"
#include "icons/destprobreroute.xpm"
#include "icons/parkingzonereroute.xpm"
#include "icons/routeprobreroute.xpm"

#include "icons/tractionsubstation.xpm"
#include "icons/overheadwire.xpm"
#include "icons/overheadwire_clamp.xpm"

#include "icons/poly.xpm"
#include "icons/poi.xpm"
#include "icons/poilane.xpm"
#include "icons/poigeo.xpm"

#include "icons/poiicons/poiicon_pin.xpm"
#include "icons/poiicons/poiicon_hotel.xpm"
#include "icons/poiicons/poiicon_nature.xpm"
#include "icons/poiicons/poiicon_fuel.xpm"
#include "icons/poiicons/poiicon_charging_station.xpm"

#include "icons/jps.xpm"
#include "icons/jps_walkablearea.xpm"
#include "icons/jps_obstacle.xpm"

#include "icons/taz.xpm"
#include "icons/tazedge.xpm"

#include "icons/container.xpm"
#include "icons/containerflow.xpm"
#include "icons/flow.xpm"
#include "icons/flowjunctions.xpm"
#include "icons/flowtazs.xpm"
#include "icons/person.xpm"
#include "icons/personflow.xpm"
#include "icons/persontripbusstop.xpm"
#include "icons/persontripchargingstation.xpm"
#include "icons/persontripcontainerstop.xpm"
#include "icons/persontripedge.xpm"
#include "icons/persontripjunction.xpm"
#include "icons/persontripparkingarea.xpm"
#include "icons/persontriptaz.xpm"
#include "icons/persontriptrainstop.xpm"
#include "icons/ridebusstop.xpm"
#include "icons/ridechargingstation.xpm"
#include "icons/ridecontainerstop.xpm"
#include "icons/rideedge.xpm"
#include "icons/ridejunction.xpm"
#include "icons/rideparkingarea.xpm"
#include "icons/ridetaz.xpm"
#include "icons/ridetrainstop.xpm"
#include "icons/route.xpm"
#include "icons/routeref.xpm"
#include "icons/routedistribution.xpm"
#include "icons/routeflow.xpm"
#include "icons/stopelement.xpm"
#include "icons/transhipbusstop.xpm"
#include "icons/transhipchargingstation.xpm"
#include "icons/transhipcontainerstop.xpm"
#include "icons/transhipedge.xpm"
#include "icons/transhipedges.xpm"
#include "icons/transhipjunction.xpm"
#include "icons/transhipparkingarea.xpm"
#include "icons/transhiptaz.xpm"
#include "icons/transhiptrainstop.xpm"
#include "icons/transportbusstop.xpm"
#include "icons/transportchargingstation.xpm"
#include "icons/transportcontainerstop.xpm"
#include "icons/transportedge.xpm"
#include "icons/transportjunction.xpm"
#include "icons/transportparkingarea.xpm"
#include "icons/transporttaz.xpm"
#include "icons/transporttrainstop.xpm"
#include "icons/trip.xpm"
#include "icons/tripjunctions.xpm"
#include "icons/triptazs.xpm"
#include "icons/vehicle.xpm"
#include "icons/vtype.xpm"
#include "icons/vtyperef.xpm"
#include "icons/vtype_bike.xpm"
#include "icons/vtype_container.xpm"
#include "icons/vtype_default.xpm"
#include "icons/vtype_pedestrian.xpm"
#include "icons/vtype_rail.xpm"
#include "icons/vtype_taxi.xpm"
#include "icons/vtypedistribution.xpm"
#include "icons/walkbusstop.xpm"
#include "icons/walkchargingstation.xpm"
#include "icons/walkcontainerstop.xpm"
#include "icons/walkedge.xpm"
#include "icons/walkedges.xpm"
#include "icons/walkjunction.xpm"
#include "icons/walkparkingarea.xpm"
#include "icons/walkroute.xpm"
#include "icons/walktaz.xpm"
#include "icons/walktrainstop.xpm"
#include "icons/waypoint.xpm"

#include "icons/dataset.xpm"
#include "icons/datainterval.xpm"
#include "icons/edgedata.xpm"
#include "icons/edgereldata.xpm"
#include "icons/tazreldata.xpm"

#include "icons/vclasses/vclass_ignoring.xpm"
#include "icons/vclasses/vclass_private.xpm"
#include "icons/vclasses/vclass_emergency.xpm"
#include "icons/vclasses/vclass_authority.xpm"
#include "icons/vclasses/vclass_army.xpm"
#include "icons/vclasses/vclass_vip.xpm"
#include "icons/vclasses/vclass_passenger.xpm"
#include "icons/vclasses/vclass_hov.xpm"
#include "icons/vclasses/vclass_taxi.xpm"
#include "icons/vclasses/vclass_bus.xpm"
#include "icons/vclasses/vclass_coach.xpm"
#include "icons/vclasses/vclass_delivery.xpm"
#include "icons/vclasses/vclass_truck.xpm"
#include "icons/vclasses/vclass_trailer.xpm"
#include "icons/vclasses/vclass_tram.xpm"
#include "icons/vclasses/vclass_rail_urban.xpm"
#include "icons/vclasses/vclass_rail.xpm"
#include "icons/vclasses/vclass_rail_electric.xpm"
#include "icons/vclasses/vclass_rail_fast.xpm"
#include "icons/vclasses/vclass_motorcycle.xpm"
#include "icons/vclasses/vclass_moped.xpm"
#include "icons/vclasses/vclass_bicycle.xpm"
#include "icons/vclasses/vclass_pedestrian.xpm"
#include "icons/vclasses/vclass_evehicle.xpm"
#include "icons/vclasses/vclass_ship.xpm"
#include "icons/vclasses/vclass_container.xpm"
#include "icons/vclasses/vclass_cable_car.xpm"
#include "icons/vclasses/vclass_subway.xpm"
#include "icons/vclasses/vclass_aircraft.xpm"
#include "icons/vclasses/vclass_wheelchair.xpm"
#include "icons/vclasses/vclass_scooter.xpm"
#include "icons/vclasses/vclass_drone.xpm"
#include "icons/vclasses/vclass_custom1.xpm"
#include "icons/vclasses/vclass_custom2.xpm"

#include "icons/vclasses/vclass_small_ignoring.xpm"
#include "icons/vclasses/vclass_small_private.xpm"
#include "icons/vclasses/vclass_small_emergency.xpm"
#include "icons/vclasses/vclass_small_authority.xpm"
#include "icons/vclasses/vclass_small_army.xpm"
#include "icons/vclasses/vclass_small_vip.xpm"
#include "icons/vclasses/vclass_small_passenger.xpm"
#include "icons/vclasses/vclass_small_hov.xpm"
#include "icons/vclasses/vclass_small_taxi.xpm"
#include "icons/vclasses/vclass_small_bus.xpm"
#include "icons/vclasses/vclass_small_coach.xpm"
#include "icons/vclasses/vclass_small_delivery.xpm"
#include "icons/vclasses/vclass_small_truck.xpm"
#include "icons/vclasses/vclass_small_trailer.xpm"
#include "icons/vclasses/vclass_small_tram.xpm"
#include "icons/vclasses/vclass_small_rail_urban.xpm"
#include "icons/vclasses/vclass_small_rail.xpm"
#include "icons/vclasses/vclass_small_rail_electric.xpm"
#include "icons/vclasses/vclass_small_rail_fast.xpm"
#include "icons/vclasses/vclass_small_motorcycle.xpm"
#include "icons/vclasses/vclass_small_moped.xpm"
#include "icons/vclasses/vclass_small_bicycle.xpm"
#include "icons/vclasses/vclass_small_pedestrian.xpm"
#include "icons/vclasses/vclass_small_evehicle.xpm"
#include "icons/vclasses/vclass_small_ship.xpm"
#include "icons/vclasses/vclass_small_container.xpm"
#include "icons/vclasses/vclass_small_cable_car.xpm"
#include "icons/vclasses/vclass_small_subway.xpm"
#include "icons/vclasses/vclass_small_aircraft.xpm"
#include "icons/vclasses/vclass_small_wheelchair.xpm"
#include "icons/vclasses/vclass_small_scooter.xpm"
#include "icons/vclasses/vclass_small_drone.xpm"
#include "icons/vclasses/vclass_small_custom1.xpm"
#include "icons/vclasses/vclass_small_custom2.xpm"

#include "icons/vshapes/vshape_pedestrian.xpm"
#include "icons/vshapes/vshape_bicycle.xpm"
#include "icons/vshapes/vshape_moped.xpm"
#include "icons/vshapes/vshape_motorcycle.xpm"
#include "icons/vshapes/vshape_passenger.xpm"
#include "icons/vshapes/vshape_passenger_sedan.xpm"
#include "icons/vshapes/vshape_passenger_hatchback.xpm"
#include "icons/vshapes/vshape_passenger_wagon.xpm"
#include "icons/vshapes/vshape_passenger_van.xpm"
#include "icons/vshapes/vshape_taxi.xpm"
#include "icons/vshapes/vshape_delivery.xpm"
#include "icons/vshapes/vshape_truck.xpm"
#include "icons/vshapes/vshape_truck_semitrailer.xpm"
#include "icons/vshapes/vshape_truck_1trailer.xpm"
#include "icons/vshapes/vshape_bus.xpm"
#include "icons/vshapes/vshape_bus_coach.xpm"
#include "icons/vshapes/vshape_bus_flexible.xpm"
#include "icons/vshapes/vshape_bus_trolley.xpm"
#include "icons/vshapes/vshape_rail.xpm"
#include "icons/vshapes/vshape_rail_car.xpm"
#include "icons/vshapes/vshape_rail_cargo.xpm"
#include "icons/vshapes/vshape_e_vehicle.xpm"
#include "icons/vshapes/vshape_ant.xpm"
#include "icons/vshapes/vshape_ship.xpm"
#include "icons/vshapes/vshape_emergency.xpm"
#include "icons/vshapes/vshape_firebrigade.xpm"
#include "icons/vshapes/vshape_police.xpm"
#include "icons/vshapes/vshape_rickshaw.xpm"
#include "icons/vshapes/vshape_scooter.xpm"
#include "icons/vshapes/vshape_aircraft.xpm"
#include "icons/vshapes/vshape_unknown.xpm"

#include "icons/accept.xpm"
#include "icons/cancel.xpm"
#include "icons/correct.xpm"
#include "icons/incorrect.xpm"
#include "icons/reset.xpm"
#include "icons/warning.xpm"
#include "icons/danger.xpm"
#include "icons/sum.xpm"
#include "icons/back.xpm"

#include "icons/error_large.xpm"
#include "icons/error_small.xpm"
#include "icons/information_large.xpm"
#include "icons/information_small.xpm"
#include "icons/question_large.xpm"
#include "icons/question_small.xpm"
#include "icons/warning_large.xpm"
#include "icons/warning_small.xpm"

#include "icons/grid.xpm"
#include "icons/grid1.xpm"
#include "icons/grid2.xpm"
#include "icons/grid3.xpm"

#include "icons/geohack.xpm"
#include "icons/googlemaps.xpm"
#include "icons/osm.xpm"

#include "icons/filedialog/bigfolder.xpm"
#include "icons/filedialog/bookclr.xpm"
#include "icons/filedialog/bookset.xpm"
#include "icons/filedialog/dirupicon.xpm"
#include "icons/filedialog/filecopy.xpm"
#include "icons/filedialog/filedelete.xpm"
#include "icons/filedialog/filehidden.xpm"
#include "icons/filedialog/filelink.xpm"
#include "icons/filedialog/filemove.xpm"
#include "icons/filedialog/fileshown.xpm"
#include "icons/filedialog/foldernew.xpm"
#include "icons/filedialog/gotohome.xpm"
#include "icons/filedialog/gotowork.xpm"
#include "icons/filedialog/showbigicons.xpm"
#include "icons/filedialog/showdetails.xpm"
#include "icons/filedialog/showsmallicons.xpm"

#include "icons/languages/de.xpm"
#include "icons/languages/es.xpm"
#include "icons/languages/pt.xpm"
#include "icons/languages/fr.xpm"
#include "icons/languages/it.xpm"
#include "icons/languages/en.xpm"
#include "icons/languages/hu.xpm"
#include "icons/languages/tr.xpm"
#include "icons/languages/zh.xpm"
#include "icons/languages/zht.xpm"
#include "icons/languages/ja.xpm"

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
    myIcons[GUIIcon::GITHUB] = new FXXPMIcon(a, github_xpm);

    myIcons[GUIIcon::VIEWDEFAULT] = new FXXPMIcon(a, viewdefault_xpm);
    myIcons[GUIIcon::VIEWJUPEDSIM] = new FXXPMIcon(a, viewjupedsim_xpm);

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
    myIcons[GUIIcon::SAVE_JUPEDSIMELEMENTS] = new FXXPMIcon(a, save_jupedsimelements_xpm);
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
    myIcons[GUIIcon::FLAG] = new FXXPMIcon(a, flag_xpm);
    myIcons[GUIIcon::FLAG_PLUS] = new FXXPMIcon(a, flag_plus_xpm);
    myIcons[GUIIcon::FLAG_MINUS] = new FXXPMIcon(a, flag_minus_xpm);
    // window arrangements
    myIcons[GUIIcon::WINDOWS_CASCADE] = new FXXPMIcon(a, windows_cascade_xpm);
    myIcons[GUIIcon::WINDOWS_TILE_VERT] = new FXXPMIcon(a, windows_tile_vertically_xpm);
    myIcons[GUIIcon::WINDOWS_TILE_HORI] = new FXXPMIcon(a, windows_tile_horizontally_xpm);
    // manipulate
    myIcons[GUIIcon::MANIP] = new FXXPMIcon(a, manip_xpm);
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
    myIcons[GUIIcon::SEARCH] = new FXXPMIcon(a, search_xpm);
    myIcons[GUIIcon::ALLOW] = new FXXPMIcon(a, allow_xpm);

    myIcons[GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID] = new FXXPMIcon(a, checkbox_grid_xpm);
    myIcons[GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE] = new FXXPMIcon(a, checkbox_drawjunctionshapes_xpm);
    myIcons[GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE] = new FXXPMIcon(a, checkbox_spreadvehicle_xpm);
    myIcons[GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS] = new FXXPMIcon(a, checkbox_showdemandelements_xpm);

    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_SELECTEDGES] = new FXXPMIcon(a, checkbox_selectededges_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_SHOWCONNECTIONS] = new FXXPMIcon(a, checkbox_showconnections_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_AUTOSELECTJUNCTIONS] = new FXXPMIcon(a, checkbox_autoselectjunctions_xpm);
    myIcons[GUIIcon::NETWORKMODE_CHECKBOX_MERGEAUTOMATICALLY] = new FXXPMIcon(a, checkbox_mergeautomatically_xpm);
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
    myIcons[GUIIcon::MODEDECAL] = new FXXPMIcon(a, modedecal_xpm);

    myIcons[GUIIcon::MODEROUTE] = new FXXPMIcon(a, moderoute_xpm);
    myIcons[GUIIcon::MODEROUTEDISTRIBUTION] = new FXXPMIcon(a, moderoutedistribution_xpm);
    myIcons[GUIIcon::MODEVEHICLE] = new FXXPMIcon(a, modevehicle_xpm);
    myIcons[GUIIcon::MODETYPE] = new FXXPMIcon(a, modetype_xpm);
    myIcons[GUIIcon::MODETYPEDISTRIBUTION] = new FXXPMIcon(a, modetypedistribution_xpm);
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

    myIcons[GUIIcon::POIICON_PIN] = new FXXPMIcon(a, poiicon_pin_xpm);
    myIcons[GUIIcon::POIICON_NATURE] = new FXXPMIcon(a, poiicon_nature_xpm);
    myIcons[GUIIcon::POIICON_HOTEL] = new FXXPMIcon(a, poiicon_hotel_xpm);
    myIcons[GUIIcon::POIICON_FUEL] = new FXXPMIcon(a, poiicon_fuel_xpm);
    myIcons[GUIIcon::POIICON_CHARGING_STATION] = new FXXPMIcon(a, poiicon_charging_station_xpm);

    myIcons[GUIIcon::JPS] = new FXXPMIcon(a, jps_xpm);
    myIcons[GUIIcon::JPS_WALKABLEAREA] = new FXXPMIcon(a, jps_walkablearea_xpm);
    myIcons[GUIIcon::JPS_OBSTACLE] = new FXXPMIcon(a, jps_obstacle_xpm);

    myIcons[GUIIcon::TAZ] = new FXXPMIcon(a, taz_xpm);
    myIcons[GUIIcon::TAZEDGE] = new FXXPMIcon(a, tazedge_xpm);

    myIcons[GUIIcon::ROUTE] = new FXXPMIcon(a, route_xpm);
    myIcons[GUIIcon::ROUTEREF] = new FXXPMIcon(a, routeref_xpm);
    myIcons[GUIIcon::ROUTEDISTRIBUTION] = new FXXPMIcon(a, routedistribution_xpm);
    myIcons[GUIIcon::VTYPE] = new FXXPMIcon(a, vtype_xpm);
    myIcons[GUIIcon::VTYPEREF] = new FXXPMIcon(a, vtyperef_xpm);
    myIcons[GUIIcon::VTYPE_DEFAULT] = new FXXPMIcon(a, vtype_default_xpm);
    myIcons[GUIIcon::VTYPE_BIKE] = new FXXPMIcon(a, vtype_bike_xpm);
    myIcons[GUIIcon::VTYPE_TAXI] = new FXXPMIcon(a, vtype_taxi_xpm);
    myIcons[GUIIcon::VTYPE_RAIL] = new FXXPMIcon(a, vtype_rail_xpm);
    myIcons[GUIIcon::VTYPE_PEDESTRIAN] = new FXXPMIcon(a, vtype_pedestrian_xpm);
    myIcons[GUIIcon::VTYPE_CONTAINER] = new FXXPMIcon(a, vtype_container_xpm);
    myIcons[GUIIcon::VTYPEDISTRIBUTION] = new FXXPMIcon(a, vtypedistribution_xpm);
    myIcons[GUIIcon::VEHICLE] = new FXXPMIcon(a, vehicle_xpm);
    myIcons[GUIIcon::TRIP] = new FXXPMIcon(a, trip_xpm);
    myIcons[GUIIcon::TRIP_JUNCTIONS] = new FXXPMIcon(a, tripjunctions_xpm);
    myIcons[GUIIcon::TRIP_TAZS] = new FXXPMIcon(a, triptazs_xpm);
    myIcons[GUIIcon::FLOW] = new FXXPMIcon(a, flow_xpm);
    myIcons[GUIIcon::FLOW_JUNCTIONS] = new FXXPMIcon(a, flowjunctions_xpm);
    myIcons[GUIIcon::FLOW_TAZS] = new FXXPMIcon(a, flowtazs_xpm);
    myIcons[GUIIcon::ROUTEFLOW] = new FXXPMIcon(a, routeflow_xpm);
    myIcons[GUIIcon::STOPELEMENT] = new FXXPMIcon(a, stopelement_xpm);
    myIcons[GUIIcon::WAYPOINT] = new FXXPMIcon(a, waypoint_xpm);
    myIcons[GUIIcon::PERSON] = new FXXPMIcon(a, person_xpm);
    myIcons[GUIIcon::PERSONFLOW] = new FXXPMIcon(a, personflow_xpm);
    myIcons[GUIIcon::PERSONTRIP_EDGE] = new FXXPMIcon(a, persontripedge_xpm);
    myIcons[GUIIcon::PERSONTRIP_BUSSTOP] = new FXXPMIcon(a, persontripbusstop_xpm);
    myIcons[GUIIcon::PERSONTRIP_TRAINSTOP] = new FXXPMIcon(a, persontriptrainstop_xpm);
    myIcons[GUIIcon::PERSONTRIP_CONTAINERSTOP] = new FXXPMIcon(a, persontripcontainerstop_xpm);
    myIcons[GUIIcon::PERSONTRIP_CHARGINGSTATION] = new FXXPMIcon(a, persontripchargingstation_xpm);
    myIcons[GUIIcon::PERSONTRIP_PARKINGAREA] = new FXXPMIcon(a, persontripparkingarea_xpm);
    myIcons[GUIIcon::PERSONTRIP_JUNCTION] = new FXXPMIcon(a, persontripjunction_xpm);
    myIcons[GUIIcon::PERSONTRIP_TAZ] = new FXXPMIcon(a, persontriptaz_xpm);
    myIcons[GUIIcon::WALK_EDGES] = new FXXPMIcon(a, walkedges_xpm);
    myIcons[GUIIcon::WALK_EDGE] = new FXXPMIcon(a, walkedge_xpm);
    myIcons[GUIIcon::WALK_BUSSTOP] = new FXXPMIcon(a, walkbusstop_xpm);
    myIcons[GUIIcon::WALK_TRAINSTOP] = new FXXPMIcon(a, walktrainstop_xpm);
    myIcons[GUIIcon::WALK_CONTAINERSTOP] = new FXXPMIcon(a, walkcontainerstop_xpm);
    myIcons[GUIIcon::WALK_CHARGINGSTATION] = new FXXPMIcon(a, walkchargingstation_xpm);
    myIcons[GUIIcon::WALK_PARKINGAREA] = new FXXPMIcon(a, walkparkingarea_xpm);
    myIcons[GUIIcon::WALK_ROUTE] = new FXXPMIcon(a, walkroute_xpm);
    myIcons[GUIIcon::WALK_JUNCTION] = new FXXPMIcon(a, walkjunction_xpm);
    myIcons[GUIIcon::WALK_TAZ] = new FXXPMIcon(a, walktaz_xpm);
    myIcons[GUIIcon::RIDE_EDGE] = new FXXPMIcon(a, rideedge_xpm);
    myIcons[GUIIcon::RIDE_BUSSTOP] = new FXXPMIcon(a, ridebusstop_xpm);
    myIcons[GUIIcon::RIDE_TRAINSTOP] = new FXXPMIcon(a, ridetrainstop_xpm);
    myIcons[GUIIcon::RIDE_CONTAINERSTOP] = new FXXPMIcon(a, ridecontainerstop_xpm);
    myIcons[GUIIcon::RIDE_CHARGINGSTATION] = new FXXPMIcon(a, ridechargingstation_xpm);
    myIcons[GUIIcon::RIDE_PARKINGAREA] = new FXXPMIcon(a, rideparkingarea_xpm);
    myIcons[GUIIcon::RIDE_JUNCTION] = new FXXPMIcon(a, ridejunction_xpm);
    myIcons[GUIIcon::RIDE_TAZ] = new FXXPMIcon(a, ridetaz_xpm);

    myIcons[GUIIcon::CONTAINER] = new FXXPMIcon(a, container_xpm);
    myIcons[GUIIcon::CONTAINERFLOW] = new FXXPMIcon(a, containerflow_xpm);
    myIcons[GUIIcon::TRANSPORT_EDGE] = new FXXPMIcon(a, transportedge_xpm);
    myIcons[GUIIcon::TRANSPORT_BUSSTOP] = new FXXPMIcon(a, transportbusstop_xpm);
    myIcons[GUIIcon::TRANSPORT_TRAINSTOP] = new FXXPMIcon(a, transporttrainstop_xpm);
    myIcons[GUIIcon::TRANSPORT_CONTAINERSTOP] = new FXXPMIcon(a, transportcontainerstop_xpm);
    myIcons[GUIIcon::TRANSPORT_CHARGINGSTATION] = new FXXPMIcon(a, transportchargingstation_xpm);
    myIcons[GUIIcon::TRANSPORT_PARKINGAREA] = new FXXPMIcon(a, transportparkingarea_xpm);
    myIcons[GUIIcon::TRANSPORT_JUNCTION] = new FXXPMIcon(a, transportjunction_xpm);
    myIcons[GUIIcon::TRANSPORT_TAZ] = new FXXPMIcon(a, transporttaz_xpm);
    myIcons[GUIIcon::TRANSHIP_EDGES] = new FXXPMIcon(a, transhipedges_xpm);
    myIcons[GUIIcon::TRANSHIP_EDGE] = new FXXPMIcon(a, transhipedge_xpm);
    myIcons[GUIIcon::TRANSHIP_BUSSTOP] = new FXXPMIcon(a, transhipbusstop_xpm);
    myIcons[GUIIcon::TRANSHIP_TRAINSTOP] = new FXXPMIcon(a, transhiptrainstop_xpm);
    myIcons[GUIIcon::TRANSHIP_CONTAINERSTOP] = new FXXPMIcon(a, transhipcontainerstop_xpm);
    myIcons[GUIIcon::TRANSHIP_CHARGINGSTATION] = new FXXPMIcon(a, transhipchargingstation_xpm);
    myIcons[GUIIcon::TRANSHIP_PARKINGAREA] = new FXXPMIcon(a, transhipparkingarea_xpm);
    myIcons[GUIIcon::TRANSHIP_JUNCTION] = new FXXPMIcon(a, transhipjunction_xpm);
    myIcons[GUIIcon::TRANSHIP_TAZ] = new FXXPMIcon(a, transhiptaz_xpm);

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
    myIcons[GUIIcon::VCLASS_CONTAINER] = new FXXPMIcon(a, vclass_container_xpm);
    myIcons[GUIIcon::VCLASS_CABLE_CAR] = new FXXPMIcon(a, vclass_cable_car_xpm);
    myIcons[GUIIcon::VCLASS_SUBWAY] = new FXXPMIcon(a, vclass_subway_xpm);
    myIcons[GUIIcon::VCLASS_AIRCRAFT] = new FXXPMIcon(a, vclass_aircraft_xpm);
    myIcons[GUIIcon::VCLASS_WHEELCHAIR] = new FXXPMIcon(a, vclass_wheelchair_xpm);
    myIcons[GUIIcon::VCLASS_SCOOTER] = new FXXPMIcon(a, vclass_scooter_xpm);
    myIcons[GUIIcon::VCLASS_DRONE] = new FXXPMIcon(a, vclass_drone_xpm);
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
    myIcons[GUIIcon::VCLASS_SMALL_CONTAINER] = new FXXPMIcon(a, vclass_small_container_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_CABLE_CAR] = new FXXPMIcon(a, vclass_small_cable_car_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_SUBWAY] = new FXXPMIcon(a, vclass_small_subway_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_AIRCRAFT] = new FXXPMIcon(a, vclass_small_aircraft_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_WHEELCHAIR] = new FXXPMIcon(a, vclass_small_wheelchair_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_SCOOTER] = new FXXPMIcon(a, vclass_small_scooter_xpm);
    myIcons[GUIIcon::VCLASS_SMALL_DRONE] = new FXXPMIcon(a, vclass_small_drone_xpm);
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

    myIcons[GUIIcon::ERROR_LARGE] = new FXXPMIcon(a, error_large_xpm);
    myIcons[GUIIcon::ERROR_SMALL] = new FXXPMIcon(a, error_small_xpm);
    myIcons[GUIIcon::INFORMATION_LARGE] = new FXXPMIcon(a, information_large_xpm);
    myIcons[GUIIcon::INFORMATION_SMALL] = new FXXPMIcon(a, information_small_xpm);
    myIcons[GUIIcon::QUESTION_LARGE] = new FXXPMIcon(a, question_large_xpm);
    myIcons[GUIIcon::QUESTION_SMALL] = new FXXPMIcon(a, question_small_xpm);
    myIcons[GUIIcon::WARNING_LARGE] = new FXXPMIcon(a, warning_large_xpm);
    myIcons[GUIIcon::WARNING_SMALL] = new FXXPMIcon(a, warning_small_xpm);

    myIcons[GUIIcon::GRID] = new FXXPMIcon(a, grid_xpm);
    myIcons[GUIIcon::GRID1] = new FXXPMIcon(a, grid1_xpm);
    myIcons[GUIIcon::GRID2] = new FXXPMIcon(a, grid2_xpm);
    myIcons[GUIIcon::GRID3] = new FXXPMIcon(a, grid3_xpm);

    myIcons[GUIIcon::GEOHACK] = new FXXPMIcon(a, geohack_xpm);
    myIcons[GUIIcon::GOOGLEMAPS] = new FXXPMIcon(a, googlemaps_xpm);
    myIcons[GUIIcon::OSM] = new FXXPMIcon(a, osm_xpm);

    myIcons[GUIIcon::FILEDIALOG_BOOK_CLR] = new FXXPMIcon(a, filedialog_bookclr_xpm);
    myIcons[GUIIcon::FILEDIALOG_BOOK_SET] = new FXXPMIcon(a, filedialog_bookset_xpm);
    myIcons[GUIIcon::FILEDIALOG_DIRUP_ICON] = new FXXPMIcon(a, filedialog_dirupicon_xpm);
    myIcons[GUIIcon::FILEDIALOG_FILE_COPY] = new FXXPMIcon(a, filedialog_filecopy_xpm);
    myIcons[GUIIcon::FILEDIALOG_FILE_DELETE] = new FXXPMIcon(a, filedialog_filedelete_xpm);
    myIcons[GUIIcon::FILEDIALOG_FILE_HIDDEN] = new FXXPMIcon(a, filedialog_filehidden_xpm);
    myIcons[GUIIcon::FILEDIALOG_FILE_LINK] = new FXXPMIcon(a, filedialog_filelink_xpm);
    myIcons[GUIIcon::FILEDIALOG_FILE_MOVE] = new FXXPMIcon(a, filedialog_filemove_xpm);
    myIcons[GUIIcon::FILEDIALOG_FILE_SHOWN] = new FXXPMIcon(a, filedialog_fileshown_xpm);
    myIcons[GUIIcon::FILEDIALOG_FOLDER_BIG] = new FXXPMIcon(a, filedialog_bigfolder_xpm);
    myIcons[GUIIcon::FILEDIALOG_FOLDER_NEW] = new FXXPMIcon(a, filedialog_foldernew_xpm);
    myIcons[GUIIcon::FILEDIALOG_GOTO_HOME] = new FXXPMIcon(a, filedialog_gotohome_xpm);
    myIcons[GUIIcon::FILEDIALOG_GOTO_WORK] = new FXXPMIcon(a, filedialog_gotowork_xpm);
    myIcons[GUIIcon::FILEDIALOG_SHOW_BIGICONS] = new FXXPMIcon(a, filedialog_showbigicons_xpm);
    myIcons[GUIIcon::FILEDIALOG_SHOW_DETAILS] = new FXXPMIcon(a, filedialog_showdetails_xpm);
    myIcons[GUIIcon::FILEDIALOG_SHOW_SMALLICONS] = new FXXPMIcon(a, filedialog_showsmallicons_xpm);

    myIcons[GUIIcon::LANGUAGE_EN] = new FXXPMIcon(a, language_en_xpm);
    myIcons[GUIIcon::LANGUAGE_DE] = new FXXPMIcon(a, language_de_xpm);
    myIcons[GUIIcon::LANGUAGE_ES] = new FXXPMIcon(a, language_es_xpm);
    myIcons[GUIIcon::LANGUAGE_PT] = new FXXPMIcon(a, language_pt_xpm);
    myIcons[GUIIcon::LANGUAGE_FR] = new FXXPMIcon(a, language_fr_xpm);
    myIcons[GUIIcon::LANGUAGE_IT] = new FXXPMIcon(a, language_it_xpm);
    myIcons[GUIIcon::LANGUAGE_HU] = new FXXPMIcon(a, language_hu_xpm);
    myIcons[GUIIcon::LANGUAGE_TR] = new FXXPMIcon(a, language_tr_xpm);
    myIcons[GUIIcon::LANGUAGE_ZH] = new FXXPMIcon(a, language_zh_xpm);
    myIcons[GUIIcon::LANGUAGE_ZHT] = new FXXPMIcon(a, language_zht_xpm);
    myIcons[GUIIcon::LANGUAGE_JA] = new FXXPMIcon(a, language_ja_xpm);

    // ... and create them
    for (const auto& icon : myIcons) {
        if (GUIDesignHeight != 23) {
            int w = (int)(icon.second->getWidth() * GUIDesignHeight / 23.0);
            int h = (int)(icon.second->getHeight() * GUIDesignHeight / 23.0);
            icon.second->scale(w, h);
        }
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
