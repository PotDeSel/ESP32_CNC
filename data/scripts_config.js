var ws = new WebSocket("ws://192.168.1.78/ws");

ws.onopen = function () {
    window.alert("Connected");
};

// Get current sensor readings when the page loads  
window.addEventListener('load', getReadings);

setInterval(Time_update, 1000);

function Time_update() {
    const dt = new Date();
    document.getElementById("time").innerHTML = dt.toLocaleTimeString();
    document.getElementById("date").innerHTML = dt.toLocaleDateString();
}

// Function to get current readings on the webpage when it loads for the first time
function getReadings() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);

            document.getElementById("DC_MOTOR_SPEED_MAX").innerHTML = myObj.DC_MOTOR_SPEED_MAX;

            document.getElementById("STEPPER_STEPS_PER_REV_X").innerHTML = myObj.STEPPER_STEPS_PER_REV_X;
            document.getElementById("STEPPER_STEPS_PER_MM_X").innerHTML = myObj.STEPPER_STEPS_PER_MM_X;
            document.getElementById("STEPPER_micropasX").innerHTML = myObj.STEPPER_micropasX;
            document.getElementById("STEPPER_pasfiltageX").innerHTML = myObj.STEPPER_pasfiltageX;

            document.getElementById("STEPPER_STEPS_PER_REV_Y").innerHTML = myObj.STEPPER_STEPS_PER_REV_Y;
            document.getElementById("STEPPER_STEPS_PER_MM_Y").innerHTML = myObj.STEPPER_STEPS_PER_MM_Y;
            document.getElementById("STEPPER_micropasY").innerHTML = myObj.STEPPER_micropasY;
            document.getElementById("STEPPER_pasfiltageY").innerHTML = myObj.STEPPER_pasfiltageY;

            document.getElementById("STEPPER_STEPS_PER_REV_Z").innerHTML = myObj.STEPPER_STEPS_PER_REV_Z;
            document.getElementById("STEPPER_STEPS_PER_MM_Z").innerHTML = myObj.STEPPER_STEPS_PER_MM_Z;
            document.getElementById("STEPPER_micropasZ").innerHTML = myObj.STEPPER_micropasZ;
            document.getElementById("STEPPER_pasfiltageZ").innerHTML = myObj.STEPPER_pasfiltageZ;

            document.getElementById("MAX_POS_X").innerHTML = myObj.MAX_POS_X;
            document.getElementById("MAX_POS_Y").innerHTML = myObj.MAX_POS_Y;
            document.getElementById("MAX_POS_Z").innerHTML = myObj.MAX_POS_Z


        }
    };
    xhr.open("GET", "/readingsConfig", true);
    xhr.send();
}



if (!!window.EventSource) {
    var source = new EventSource('/events');

    source.addEventListener('open', function (e) {
        console.log("Events Connected");
    }, false);

    source.addEventListener('error', function (e) {
        if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
        }
    }, false);

    source.addEventListener('message', function (e) {
        //console.log("message", e.data);
    }, false);

    source.addEventListener('new_readings', function (e) {
        //console.log("new_readings", e.data);
        var myObj = JSON.parse(e.data);
        //console.log(myObj);

        document.getElementById("DC_MOTOR_SPEED_MAX").innerHTML = myObj.DC_MOTOR_SPEED_MAX;

        document.getElementById("STEPPER_STEPS_PER_REV_X").innerHTML = myObj.STEPPER_STEPS_PER_REV_X;
        document.getElementById("STEPPER_STEPS_PER_MM_X").innerHTML = myObj.STEPPER_STEPS_PER_MM_X;
        document.getElementById("STEPPER_micropasX").innerHTML = myObj.STEPPER_micropasX;
        document.getElementById("STEPPER_pasfiltageX").innerHTML = myObj.STEPPER_pasfiltageX;

        document.getElementById("STEPPER_STEPS_PER_REV_Y").innerHTML = myObj.STEPPER_STEPS_PER_REV_Y;
        document.getElementById("STEPPER_STEPS_PER_MM_Y").innerHTML = myObj.STEPPER_STEPS_PER_MM_Y;
        document.getElementById("STEPPER_micropasY").innerHTML = myObj.STEPPER_micropasY;
        document.getElementById("STEPPER_pasfiltageY").innerHTML = myObj.STEPPER_pasfiltageY;

        document.getElementById("STEPPER_STEPS_PER_REV_Z").innerHTML = myObj.STEPPER_STEPS_PER_REV_Z;
        document.getElementById("STEPPER_STEPS_PER_MM_Z").innerHTML = myObj.STEPPER_STEPS_PER_MM_Z;
        document.getElementById("STEPPER_micropasZ").innerHTML = myObj.STEPPER_micropasZ;
        document.getElementById("STEPPER_pasfiltageZ").innerHTML = myObj.STEPPER_pasfiltageZ;

        document.getElementById("MAX_POS_X").innerHTML = myObj.MAX_POS_X;
        document.getElementById("MAX_POS_Y").innerHTML = myObj.MAX_POS_Y;
        document.getElementById("MAX_POS_Z").innerHTML = myObj.MAX_POS_Z
    }, false);

}