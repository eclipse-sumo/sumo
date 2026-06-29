on("ready", function(){
    const params = new URLSearchParams(globalThis.location.search);
    const PORT = params.has("port") ? params.get("port") : "8010";

    /**
     * @class
     * @param display: the display string
     * @param internal: the internal string, like the VClass string in SUMO
     * @param fringeFactor: initial fringe factor value
     * @param count: initial count value
     */
    function Settings(){
        this.init.apply(this, arguments);
    }

    Settings.prototype = {
        init: function(display, internal, fringeFactor, count, enabled){
            this.internal = internal;

            let node = elem("<div>", {className: "container"});
            let header = elem("<h4>", {textContent: display});
            header.append("<img>", {src: "images/" + internal + ".png"});
            node.append(header);
            this.enable = elem("<input>", {type: "checkbox", checked: enabled});
            node.append(this.enable);

            let options = elem("<div>", {className: "options"});
            let label = elem("<label>", {textContent: "Through Traffic Factor"});
            this.fringeFactor = elem("<input>", {type: "number", min: .5, max: 100, step: .1, value: fringeFactor});
            label.append(this.fringeFactor);
            options.append(label);

            label = elem("<label>", {textContent: "Count", title: "Count per hour per kilometer"});
            this.count = elem("<input>", {type: "number", min: .2, max: 100, step: .1, value: count});
            label.append(this.count);
            options.append(label);

            node.append(options);
            elem("#vehicle-controls").append(node);
        },

        toJSON: function(){
            if(this.enable.checked){
                return {
                    fringeFactor: Number.parseFloat(this.fringeFactor.value),
                    count: Number.parseFloat(this.count.value)
                };
            }
            return null;
        }
    };

    const vehicleClasses = [
        new Settings("Cars", "passenger", 5, 12, true),
        new Settings("Trucks", "truck", 5, 8),
        new Settings("Bus", "bus", 5, 4),
        new Settings("Motorcycles", "motorcycle", 2, 4),
        new Settings("Bicycles", "bicycle", 2, 6),
        new Settings("Pedestrians", "pedestrian", 1, 10),
        new Settings("Trams", "tram", 20, 2),
        new Settings("Urban trains", "rail_urban", 40, 2),
        new Settings("Trains", "rail", 40, 2),
        new Settings("Ships", "ship", 40, 2)
    ];

    function RoadTypes(){
        this.init.apply(this, arguments);
    }

    RoadTypes.prototype = {
        init: function (category, typeList) {
            this.category = category;
            this.typeList = typeList;

            let node = elem("<div>", {className: "container"});
            let header = elem("<h4>", {textContent: category});
            let checkbox = elem("<input>",{type: "checkbox", checked:true, className: "checkAll", id: category.toLowerCase()});
            node.append(header);
            node.append(checkbox);

            let types = elem("<div>", {className: "roadTypes " + category.toLowerCase()});
            let label = elem("<label>");

            for (let i = 0; i < typeList.length; i++) {
                label = elem("<label>", {textContent: typeList[i]});
                let roadTypeId = this.category + "_" + typeList[i]
                this.roadTypeCheck = elem("<input>",{type: "checkbox", checked:true, id: roadTypeId});

                label.append(this.roadTypeCheck);
                types.append(label);
            }

            node.append(types);
            elem("#road-types").append(node);
        },

        getEnabledTypeList: function () {
            let retEnabledTypeList = [];
            for (let j = 0; j < this.typeList.length; j++) {
                let roadTypeId = this.category + "_" + this.typeList[j];
                if (document.getElementById(roadTypeId).checked) {
                    retEnabledTypeList.push(this.typeList[j]);
                    if (this.typeList[j].match(/^(motorway|trunk|primary|secondary|tertiary)$/)) {
                        retEnabledTypeList.push(this.typeList[j] + "_link");
                    }
                }
            }
            return retEnabledTypeList;
        }
    };

    const categories = {};
    categories["Highway"] = ["motorway", "trunk", "primary","secondary", "tertiary", "unclassified", "residential",
        "living_street", "unsurfaced", "service", "raceway", "bus_guideway"];
    categories["Pedestrians"] = ["track", "footway", "pedestrian", "path", "bridleway", "cycleway", "step", "steps",
        "stairs"]; //"Pedestrians" has also the "highway" key in OSM, this will be transformed in startBuild()
    categories["Railway"] = ["preserved", "tram", "subway", "light_rail", "rail", "highspeed", "monorail"];
    categories["Aeroway"] = ["stopway", "parking_position", "taxiway", "taxilane", "runway", "highway_strip"]
    categories["Waterway"] = ["river", "canal"];
    categories["Aerialway"] = ["cable_car", "gondola"];
    categories["Route"] = ["ferry"];

    let roadClasses = [];

    for (const [key, value] of Object.entries(categories)) {
        roadClasses.push(new RoadTypes(key, value));
    }

    let activeTab = null;

    /**
     * @function
     * @param id: the id of the tab to open
     */
    function openTab(id){
        let tab = elems(".tab")[id];
        let side = elem("#side");
        let control = elems(".controls")[id];

        // clicked on the open tab, close everything
        if(activeTab === id){
            side.classList.remove("open");
            control.classList.remove("open");
            tab.classList.remove("open");
            activeTab = null;
        } else {
            // open the side and control
            side.classList.add("open");
            control.classList.add("open");
            tab.classList.add("open");

            // close the other tab, if there is one
            if(activeTab !== null){
                elems(".controls")[activeTab].classList.remove("open");
                elems(".tab")[activeTab].classList.remove("open");
            }

            activeTab = id;
        }
    }

    elems(".tab").forEach(function(tab, index){
        tab.dataset.symbol = tab.textContent;
        tab.on("click", function(){
            openTab(index);
        });
    });

    openTab(0);

    let canvas = elem("canvas");
    let canvasActive = false;
    let canvasRect = [.1, .1, .75, .9];
    let ctx = canvas.getContext("2d");

    /**
     * @function
     * sets the canvas to full page size
     */
    function setCanvasSize(){
        canvas.width = innerWidth;
        canvas.height = innerHeight;
        draw();
    }

    setCanvasSize();
    on("resize", setCanvasSize);

    /**
     * @function
     * draws the rect on the canvas, to select an area
     **/
    function draw(){
        let x0 = canvas.width * canvasRect[0],
            y0 = canvas.height * canvasRect[1],
            x1 = canvas.width * canvasRect[2],
            y1 = canvas.height * canvasRect[3];

        ctx.clearRect(0, 0, canvas.width, canvas.height);

        ctx.fillStyle = "#808080";
        ctx.globalAlpha = .5;

        ctx.fillRect(0, 0, canvas.width, canvas.height);
        ctx.clearRect(x0, y0, x1 - x0, y1 - y0);
    }

    let mouse = {
        x: 0, // last x coordinate
        y: 0, // last y coordinates
        area: null // if mouse button is down, and in which function; move or resize
    }

    canvas.on("mousedown", function(evt){
        mouse.area = changeMousePointer(evt.clientX, evt.clientY, true);
    });
    canvas.on("mouseup", function(evt){
        mouse.area = null;
        changeMousePointer(evt.clientX, evt.clientY);
    });

    function changeMousePointer(x, y, down){
        let x0 = canvas.width * canvasRect[0],
            y0 = canvas.height * canvasRect[1],
            x1 = canvas.width * canvasRect[2],
            y1 = canvas.height * canvasRect[3];

        let cursor = "", t = 20; //tolerance

        if(lequal(x0 - t, x, x1 + t) && lequal(y0 - t, y, y1 + t)){
            if(lequal(y0 - t, y, y0 + t))
                cursor += "n";
            else if(lequal(y1 - t, y, y1 + t))
                cursor += "s";

            if(lequal(x0 - t, x, x0 + t))
                cursor += "w";
            else if(lequal(x1 - t, x, x1 + t))
                cursor += "e";

            if(cursor) cursor += "-resize";
            else if(mouse.area || down) cursor = "move";
            else cursor = "pointer";
        } else
            cursor = "auto"

        canvas.style.cursor = cursor;
        return cursor.match(/^([nsm])?([we])?/);
    }

    canvas.on("mousemove", function(evt){
        changeMousePointer(evt.clientX, evt.clientY);
        let dx = (evt.clientX - mouse.x) / canvas.width, dy = (evt.clientY - mouse.y) / canvas.height;
        mouse.x = evt.clientX;
        mouse.y = evt.clientY;
        if(mouse.area !== null){
            if(mouse.area[1] == "n"){
                if((canvasRect[1] + dy)<=canvasRect[3]){
                    canvasRect[1] += dy;
                } else if ((canvasRect[1] + dy)>canvasRect[3]){
                    [canvasRect[1], canvasRect[3]] = [canvasRect[3], canvasRect[1]];
                    canvasRect[3] += dy;
                    mouse.area[1] = "s";
                }
            }
            else if(mouse.area[1] == "s"){
                if((canvasRect[3] + dy)>=canvasRect[1]){
                    canvasRect[3] += dy;
                } else if ((canvasRect[3] + dy)<canvasRect[1]){
                    [canvasRect[1], canvasRect[3]] = [canvasRect[3], canvasRect[1]];
                    canvasRect[1] += dy;
                    mouse.area[1] = "n";
                }
            }

            if(mouse.area[2] == "w"){
                if((canvasRect[0] + dx)<=canvasRect[2]){
                    canvasRect[0] += dx;
                } else if ((canvasRect[0] + dx)>canvasRect[2]){
                    [canvasRect[0], canvasRect[2]] = [canvasRect[2], canvasRect[0]];
                    canvasRect[2] += dx;
                    mouse.area[2] = "e";
                }
            }
            else if(mouse.area[2] == "e"){
                if((canvasRect[2] + dx)>=canvasRect[0]){
                    canvasRect[2] += dx;
                } else if ((canvasRect[2] + dx)<canvasRect[0]){
                    [canvasRect[0], canvasRect[2]] = [canvasRect[2], canvasRect[0]];
                    canvasRect[0] += dx;
                    mouse.area[2] = "w";
                }
            }

            if(mouse.area[1] == "m"){
                canvasRect[0] += dx;
                canvasRect[1] += dy;
                canvasRect[2] += dx;
                canvasRect[3] += dy;
            }

            draw();
        }
    });

    /**
     * @function
     * checks if the checkbox is checked and displays the canvas in this case, else not
     **/
    function toggleCanvas(){
        if(canvasToggle.checked){
            canvasActive = true;
            canvas.style.removeProperty("display");
        } else {
            canvasActive = false;
            canvas.style.display = "none";
        }
    }

    const canvasToggle = elem("#canvas-toggle");

    canvasToggle.on("click", toggleCanvas);
    toggleCanvas();

    // Enable/disable pt mode checkboxes depending on the main publicTransport checkbox
    let ptModesContainer = elem("#pt-modes");
    function updatePtModesState() {
        let enabled = elem("#publicTransport").checked;
        if(ptModesContainer) {
            Array.from(ptModesContainer.querySelectorAll("input[type=checkbox]")).forEach(function(ch){ ch.disabled = !enabled; });
        }
    }
    elem("#publicTransport").on("click", updatePtModesState);
    updatePtModesState();

    // function to check or uncheck all checkboxes for a certain roadType
    let checkOrUncheckAll = function() {
        Array.from(document.querySelectorAll(".roadTypes." + this.getAttribute("id") + " input[type=checkbox]")).forEach(el => el.checked = this.checked);
    };

    // listen if a roadType checkbox is selected/unselected
    let roadTypeCheckboxes = document.getElementsByClassName("checkAll");

    Array.from(roadTypeCheckboxes).forEach(function(element) {
        element.addEventListener("click", checkOrUncheckAll);
    });

    // Base map (using Leaflet)
    const worldBounds = L.latLngBounds(
        L.latLng(-85, -180),
        L.latLng(85, 180)
    );

    let map = L.map("map", {
        maxBounds: worldBounds,
        worldCopyJump: false,
        zoomControl: true,
        minZoom: 3,
        maxZoom: 17, // the highest zoom level available for the OpenTopoMap tile layer
        attributionControl: false
    });

    L.control.attribution({
        position: "bottomleft",
        prefix: false
    }).addTo(map);

    const tileLayers = {
        "OpenStreetMap": L.tileLayer("https://tile.openstreetmap.org/{z}/{x}/{y}.png", {
            noWrap: true,
            attribution: '&copy; <a href="https://www.openstreetmap.org/copyright" target="_blank">OpenStreetMap contributors</a>'
        }),
        "OpenStreetMap Deutschland": L.tileLayer("https://tile.openstreetmap.de/{z}/{x}/{y}.png", {
            noWrap: true,
            attribution: '&copy; <a href="https://www.openstreetmap.org/copyright" target="_blank">OpenStreetMap contributors</a>'
        }),
        "OpenTopoMap": L.tileLayer("https://a.tile.opentopomap.org/{z}/{x}/{y}.png", {
            noWrap: true,
            attribution: 'Map data: &copy; <a href="https://www.openstreetmap.org/copyright" target="_blank">OpenStreetMap contributors</a> | DEM: <a href="http://viewfinderpanoramas.org/" target="_blank">SRTM</a>, <a href="https://sonny.4lima.de/" target="_blank">Sonny</a> | Map style: &copy; <a href="https://opentopomap.org" target="_blank">OpenTopoMap</a> (<a href="https://creativecommons.org/licenses/by-sa/3.0/" target="_blank">CC-BY-SA</a>)'
        }),
        "ÖPNVKarte (public transport facilities)": L.tileLayer("https://tile.memomaps.de/tilegen/{z}/{x}/{y}.png", {
            noWrap: true,
            attribution: '&copy; <a href="https://www.openstreetmap.org/copyright" target="_blank">OpenStreetMap contributors</a> | &copy; <a href="https://memomaps.de/" target="_blank">MeMoMaps</a> (<a href="https://creativecommons.org/licenses/by-sa/2.0/" target="_blank">CC-BY-SA</a>)'
        }),
    };

    tileLayers["OpenStreetMap"].addTo(map);

    L.control.layers(tileLayers, null, {
        collapsed: true,
        position: "topleft"
    }).addTo(map);

    function setPosition(lon, lat){
        if(!lon || !lat){
            let latLon = elem("#lat_lon").value.split(" ");
            lon = Number.parseFloat(latLon[1]);
            lat = Number.parseFloat(latLon[0]);
        } else {
            elem("#lat_lon").value = lat.toFixed(6) + " " + lon.toFixed(6);
        }

        const leftHandBounds = [
            [[50, -11], [60, 1]], // British Isles
            [[50.986099, 0.774536], [53.14677, 1.779785]], // British Isles (part2)
            [[3, 66], [30, 90]], // India, Pakistan
            [[-45, 95], [2, 179]], // Australia, Indonesia
            [[-35, -20], [-15, 40]], // Southern Africa
            [[30, 135], [42, 150]] // Japan
        ];

        elem("#leftHand").checked = false;
        for (let i = 0; i < leftHandBounds.length; i++) {
            if (
                lat >= leftHandBounds[i][0][0] && lat <= leftHandBounds[i][1][0] &&
                lon >= leftHandBounds[i][0][1] && lon <= leftHandBounds[i][1][1]
            ) {
                elem("#leftHand").checked = true;
                break;
            }
        }

        map.setView([lat, lon], 16);
    }

    function setPositionByString() {
        let query = elem("#address").value;
        $.ajax({
        url: "https://nominatim.openstreetmap.org/search?q=" + query + "&format=json&polygon=0&addressdetails=0&limit=1&callback",
        cache: false,
        dataType: "json",
            success: function(data) {
                if (!Array.isArray(data) || data.length === 0) {
                    globalThis.alert('Could not locate address: ' + query);
                    return;
                }
                let result = data[0];
                let lon = Number.parseFloat(result.lon);
                let lat = Number.parseFloat(result.lat);
                if (Number.isNaN(lon) || Number.isNaN(lat)) {
                    globalThis.alert('Could not locate address: ' + query);
                    return;
                }
                setPosition(lon, lat);
            },
            error: function (request, status, err) {
                globalThis.alert('Could not locate address: ' + err);
            }
        });
    }

    elem("#address").on("keyup", function(e){
        if (e.keyCode == 13) {
            setPositionByString();
        }
    });

/*     let getJSON = function(url, callback) {
        let xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        xhr.responseType = 'json';
        xhr.onload = function() {
          let status = xhr.status;
          if (status == 200) {
            callback(null, xhr.response);
          } else {
            callback(status);
          }
        };
        xhr.send();
    }; */

    // set default position to center Berlin
    setPosition(13.4, 52.52);

    /**
     * @listener
     * set the coordinates of the map to current coordinates (got from browser)
     */
    elem("#buttonCurrent").on("click", function(){
        if(!navigator.geolocation) {
            globalThis.alert("Unable to access your location. Geolocation is either not supported by your browser or is not enabled.");
            return;
        }

        navigator.geolocation.getCurrentPosition(function(position){
            setPosition(position.coords.longitude, position.coords.latitude);
        });
    });

    /**
     * @listener
     * whenever the input boxes changes, update the map coordinates
     */
    elem("#buttonSearch").on("click", setPositionByString);
    elem("#buttonLatLon").on("click", setPosition);

    /**
     * @listener
     * whenever the map coordinates changes, update the input boxes
     */
    map.on("move", function(){
        let center = map.getCenter();
        let lat = center.lat;
        let lon = center.lng;

        elem("#lat_lon").value = lat.toFixed(6) + " " + lon.toFixed(6);
    });

    let socket;
    let totalSteps;
    let currentStep;
    let presentedErrorLog = false;
    let progressTimer = null;

    function errorMessage(error) {
        if (presentedErrorLog == false) {
            globalThis.alert("Server connection failed (" + error + "). Please (re-)open the OSM Web Wizard by using osmWebWizard.py or the link in your start menu.");
            presentedErrorLog = true;
        }
    }

    async function safeFetch(url, options) {
        try {
            const response = await fetch(url, options);
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}`);
            }
            return response;
        } catch (err) {
            errorMessage(err);
            throw err;
        }
    }

    /**
     * @function
     * connects to the socket, when it fails it tries it again after five seconds
     */
    function connectSocket(){
        let address = location.hostname;
        // when accessing via file, location.hostname is an empty string, so guess that the server is on localhost
        if(!address)
            address = "localhost";
        try {
            // we use this locally but if someone wants to try over https, we are prepared
            let proto = (globalThis.location.protocol === "https:") ? "wss://" : "ws://";
            socket = new WebSocket(proto + address + ":" + PORT);
        } catch(e){
            // connection failed, wait five seconds, then try again
            setTimeout(connectSocket, 5000);
            return;
        }

        socket.onerror = errorMessage;

        // whenever the socket closes (e.g. restart) try to reconnect
        socket.addEventListener("close", connectSocket);
        socket.addEventListener("message", messageHandler);
    }

    function messageHandler(evt){
        let message = evt.data;
        // get the first space
        let index = message.indexOf(" ");
        // split the message type from the message
        let type = message.substr(0, index);
        message = message.substr(index + 1);

        if(type === "zip"){
            showZip(message);
        } else if(type === "report"){
            currentStep++;
            elem("#status > span").textContent = message;
            elem("#status > div").style.width = (100 * currentStep / totalSteps) + "%";

            if(currentStep === totalSteps){
                setTimeout(function(){
                    elem("#status").style.display = "none";
                    elem("#export-button").style.display = "block";
                }, 2000);
                if (progressTimer) {
                    clearInterval(progressTimer);
                    progressTimer = null;
                }
            }
        } else if(type === "steps"){
            totalSteps = Number.parseInt(message);
            currentStep = 0;
        }
    }

    if (globalThis.location.protocol == "file:") {
        connectSocket();
    }

    async function pollProgress() {
        const r = await fetch("/progress");
        const d = await r.json();
        if (d.data) {
            messageHandler(d);
        }
    }

    /**
     * @function
     * generate and send the data to the websocket
     */
    function startBuild(){
        let bounds = map.getBounds();
        let cor = {
            left: bounds.getWest(),
            bottom: bounds.getSouth(),
            right: bounds.getEast(),
            top: bounds.getNorth()
        };

        let data = {
            poly: elem("#polygons").checked,
            duration: Number.parseInt(elem("#duration").value),
            publicTransport: elem("#publicTransport").checked,
            ptModes: {
                bus: elem("#pt_bus").checked,
                tram: elem("#pt_tram").checked,
                subway: elem("#pt_subway").checked,
                light_rail: elem("#pt_light_rail").checked,
                train: elem("#pt_train").checked,
                trolleybus: elem("#pt_trolleybus").checked,
                monorail: elem("#pt_monorail").checked,
                minibus: elem("#pt_minibus").checked,
                share_taxi: elem("#pt_share_taxi").checked,
                ferry: elem("#pt_ferry").checked,
                aerialway: elem("#pt_aerialway").checked
            },
            leftHand: elem("#leftHand").checked,
            decal: elem("#decal").checked,
            carOnlyNetwork: elem("#carOnlyNetwork").checked,
            verbose: elem("#verbose").checked,
            vehicles: {},
            roadTypes:{} // sab-inf
        };

        // calculates the coordinates of the rectangle if area-picking is active
        if(canvasActive){
            let width = cor.right - cor.left;
            let height = cor.bottom - cor.top;
            data.coords = [
                cor.left + width * canvasRect[0],
                cor.top + height * canvasRect[3],
                cor.left + width * canvasRect[2],
                cor.top + height * canvasRect[1]
            ];
        } else
            data.coords = [cor.left, cor.bottom, cor.right, cor.top];

        vehicleClasses.forEach(function(vehicleClass){
            let result = vehicleClass.toJSON();
            if(result)
                data.vehicles[vehicleClass.internal] = result;
        });

        roadClasses.forEach(function(roadType){
            let result = roadType.getEnabledTypeList();
            if(result){
                // in OSM "pedestrians" have also the key "highway", therefore we prepare the data accordingly
                if(roadType.category == "Pedestrians" || roadType.category == "Highway"){
                    try{
                        data.roadTypes["Highway"] = data.roadTypes["Highway"].concat(result);
                    }catch (e){
                        if (e instanceof TypeError) {
                            data.roadTypes["Highway"] = result;
                        }
                    }
                }else
                    data.roadTypes[roadType.category] = result;
            }
        });

        if (globalThis.location.protocol == "file:") {
            try {
                socket.send(JSON.stringify(data));
            } catch(e){
                return;
            }
        } else {
            safeFetch("/build", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify(data)
            })
            // poll for messages
            progressTimer = setInterval(pollProgress, 500);
        }

        elem("#status").style.display = "block";
        elem("#export-button").style.display = "none";
    }

    elem("#export-button").on("click", startBuild);

    /**
     * @function
     * @param uri: a base64-encoded string for a zip file
     * shows a download dialog to save the zip file
     */
    function showZip(uri){
        let url = "data:application/zip;base64," + uri;

        // using a temporarily link to trigger the download dialog
        let link = elem("<a>", {
            download: "osm.zip",
            href: url,
            target: "_blank"
        });

        document.body.appendChild(link);
        link.click();
        link.remove();
    }
});
