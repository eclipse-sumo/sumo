on("ready", function(){
    elem("#export-button").on("click", function(){
        var cor = map.getExtent();
        cor.transform(
            map.getProjectionObject(), // from Spherical Mercator Projection
            new OpenLayers.Projection("EPSG:4326")
        );

        //calculates the coordinates of the rectangle if area-picking is active
        if(canvasActive){
            var width = cor.right - cor.left;
            var height = cor.bottom - cor.top;
            cor = [
                cor.top + height * canvasRect[3],
                cor.left + width * canvasRect[0],
                cor.top + height * canvasRect[1],
                cor.left + width * canvasRect[2]
            ];
        } else
            cor = [cor.bottom, cor.left, cor.top, cor.right];

        socket.send("coords: " + cor.join(","));
    });

    /**
     * @param de: the display string
     * @param en: the internal string
     */
    function Settings(de, en, fringeFactor){
        this._init(de, en, fringeFactor);
    }
    Settings.prototype = {
        _init: function(de, en, fringeFactor){
            var node = elem("<span>", {className: "container", textContent: de});
            var checkbox = elem("<input>", {type: "checkbox"});
            checkbox.on("click", function(evt){
                socket.send(en + ": " + (evt.target.checked? "+" : ""));
            });
            node.append(checkbox);

            var options = elem("<div>", {className: "options"});
            var label = elem("<label>", {textContent: "Durchgangsverkehr"});
            var input = elem("<input>", {type: "number", min: .5, max: 100, step: .1, value: fringeFactor});
            input.on("input", function(evt){
                socket.send(en + "FringeFactor: " + evt.target.value);
            });
            label.append(input);
            options.append(label);

            label = elem("<label>", {textContent: "Frequenz"});
            input = elem("<input>", {type: "number", min: .2, max: 100, step: .1, value: 10});
            input.on("input", function(evt){
                socket.send(en + "Period: " + evt.target.value);
            });
            label.append(input);
            options.append(label);

            label = elem("<label>", {textContent: "Dauer"});
            input = elem("<input>", {type: "number", min: 60, max: 36000, step: 60, value: 3600});
            input.on("input", function(evt){
                socket.send(en + "Time: " + evt.target.value);
            });
            label.append(input);
            options.append(label);

            node.append(options);
            elem("#controls").append(node);
        }
    };

    new Settings("Fahrzeuge", "vehicles", 5);
    new Settings("Fahrradfahrer", "bicycles", 2);
    new Settings("Fußgänger", "pedestrians", 1);
    new Settings("Züge", "rails", 40);

    var canvas = elem("canvas");
    var canvasActive = false,
        canvasRect = [.1, .1, .9, .9];
    var ctx = canvas.getContext("2d");

    //to set canvas fullpage
    function setCanvasSize(){
        canvas.width = innerWidth;
        canvas.height = innerHeight;
        draw();
    }
    setCanvasSize();
    on("resize", setCanvasSize);

    /**
     * @function draw: draws the rect on the canvas.
     **/
    function draw(){
        var x0 = canvas.width * canvasRect[0],
            y0 = canvas.height * canvasRect[1],
            x1 = canvas.width * canvasRect[2],
            y1 = canvas.height * canvasRect[3];

        ctx.clearRect(0, 0, canvas.width, canvas.height);

        ctx.fillStyle = "#A5E5FF";
        ctx.strokeStyle = "#61C4F6";
        ctx.lineWidth = 5;
        ctx.globalAlpha = .4;
        ctx.beginPath();
        ctx.rect(x0, y0, x1 - x0, y1 - y0);
        ctx.fill();
        ctx.stroke();
    }

    var mouse = {
        x: 0, //last x coordinate
        y: 0, //last y coordinates
        area: null //if mouse button is down, and in which function; move or resize
    }

    canvas.on("mousedown", function(evt){
        mouse.area = changeMousePointer(evt.clientX, evt.clientY, true);
    });
    canvas.on("mouseup", function(evt){
        mouse.area = null;
        changeMousePointer(evt.clientX, evt.clientY);
    });

    function changeMousePointer(x, y, down){
        var x0 = canvas.width * canvasRect[0],
            y0 = canvas.height * canvasRect[1],
            x1 = canvas.width * canvasRect[2],
            y1 = canvas.height * canvasRect[3];

        var cursor = "", t = 20; //tolerance

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
        var dx = (evt.clientX - mouse.x) / canvas.width, dy = (evt.clientY - mouse.y) / canvas.height;
        mouse.x = evt.clientX;
        mouse.y = evt.clientY;
        if(mouse.area !== null){
            if(mouse.area[1] == "n")
                canvasRect[1] += dy;
            else if(mouse.area[1] == "s")
                canvasRect[3] += dy;

            if(mouse.area[2] == "w")
                canvasRect[0] += dx;
            else if(mouse.area[2] == "e")
                canvasRect[2] += dx;

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
     * @function toogleCanvas: checks if the checkbox is checked and displays the canvas in this case, else not
     **/
    function toogleCanvas(){
        if(canvasToogle.checked){
            canvasActive = true;
            canvas.style.removeProperty("display");
        } else {
            canvasActive = false;
            canvas.style.display = "none";
        }
    }

    var canvasToogle = elem("#canvas-toogle");

    canvasToogle.on("click", toogleCanvas);
    toogleCanvas();

    //initiales the WebSocket. If you change the port (9999) here, change it also in the python file
    socket = new WebSocket("ws://localhost:9999");

    socket.addEventListener("open", function(){
        elem("#controls").style.removeProperty("display");
    });

    //OSM map
    map = new OpenLayers.Map("map");
    map.addLayer(new OpenLayers.Layer.OSM());

    function setPosition(lon, lat){
        if(!lon || !lat){
            lon = parseFloat(elem("#current-lon").value);
            lat = parseFloat(elem("#current-lat").value);
        } else {
            elem("#current-lon").value = lon;
            elem("#current-lat").value = lat;
        }

        var lonLat = new OpenLayers.LonLat(lon, lat)
            .transform(
                    new OpenLayers.Projection("EPSG:4326"), // transform from WGS 1984
                    map.getProjectionObject() // to Spherical Mercator Projection
            );

        map.setCenter(lonLat, 16);
    }

    setPosition(13.4, 52.52);

    elem("#set-position").on("click", function(){
        if(!navigator.geolocation) return;

        navigator.geolocation.getCurrentPosition(function(position){
            setPosition(position.coords.longitude, position.coords.latitude);
        });
    });

    elem("#current-lon").on("input", setPosition);
    elem("#current-lat").on("input", setPosition);

    elem("#position").on("mouseenter", function(){
        var cor = map.getExtent();
        cor.transform(
            map.getProjectionObject(), // from Spherical Mercator Projection
            new OpenLayers.Projection("EPSG:4326")
        );

        elem("#current-lon").value = cor.left + (cor.right - cor.left) / 2;
        elem("#current-lat").value = cor.top + (cor.bottom - cor.top) / 2;
    });
});
