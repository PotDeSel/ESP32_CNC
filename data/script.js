var ws = new WebSocket("ws://192.168.1.78/ws");

ws.onopen = function () {
  window.alert("Connected");
};

// Get current sensor readings when the page loads  
window.addEventListener('load', getReadings);

// Function to get current readings on the webpage when it loads for the first time
function getReadings() {
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      var myObj = JSON.parse(this.responseText);
      document.getElementById('DC_analog').innerHTML = myObj.pwm;
      document.getElementById('STEPPER_vitesseX').innerHTML = myObj.STEPPER_speedX;
      document.getElementById('STEPPER_vitesseY').innerHTML = myObj.STEPPER_speedY;
      document.getElementById('STEPPER_vitesseZ').innerHTML = myObj.STEPPER_speedZ;

      document.getElementById('STEPPER_absolueX').innerHTML = myObj.STEPPER_absolueX;
      document.getElementById('STEPPER_relativeX').innerHTML = myObj.STEPPER_relativeX;
      document.getElementById('STEPPER_absolueY').innerHTML = myObj.STEPPER_absolueY;
      document.getElementById('STEPPER_relativeY').innerHTML = myObj.STEPPER_relativeY;
      document.getElementById('STEPPER_absolueZ').innerHTML = myObj.STEPPER_absolueZ;
      document.getElementById('STEPPER_relativeZ').innerHTML = myObj.STEPPER_relativeZ;

      gaugeSpeed.value = myObj.gauge;

      document.getElementById("Work").classList.add("Green_X");
    }
  };
  xhr.open("GET", "/readingsMain", true);
  xhr.send();
}

setInterval(Time_update, 1000);

function Time_update() {
  const dt = new Date();
  document.getElementById("time").innerHTML = dt.toLocaleTimeString();
  document.getElementById("date").innerHTML = dt.toLocaleDateString();
}

// Créer la gauge
var gaugeSpeed = new RadialGauge({
  renderTo: 'gauge-speed',
  width: 300,
  height: 300,
  units: "Vitesse (tr/min)",
  minValue: 0,
  maxValue: 4000,
  colorValueBoxRect: "#049faa",
  colorValueBoxRectEnd: "#049faa",
  colorValueBoxBackground: "#f1fbfc",
  valueInt: 2,
  majorTicks: [
    "0",
    "500",
    "1000",
    "1500",
    "2000",
    "2500",
    "3000",
    "3500",
    "4000"
  ],
  minorTicks: 4,
  strokeTicks: true,
  highlights: [
    {
      "from": 80,
      "to": 100,
      "color": "#03C0C1"
    }
  ],
  colorPlate: "#fff",
  borderShadowWidth: 0,
  borders: false,
  needleType: "line",
  colorNeedle: "#007F80",
  colorNeedleEnd: "#007F80",
  needleWidth: 2,
  needleCircleSize: 3,
  colorNeedleCircleOuter: "#007F80",
  needleCircleOuter: true,
  needleCircleInner: false,
  animationDuration: 500,
  animationRule: "linear"
}).draw();

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
    gaugeSpeed.value = myObj.gauge;
    document.getElementById('DC_analog').innerHTML = myObj.pwm;
    document.getElementById('STEPPER_vitesseX').innerHTML = myObj.STEPPER_speedX;
    document.getElementById('STEPPER_vitesseY').innerHTML = myObj.STEPPER_speedY;
    document.getElementById('STEPPER_vitesseZ').innerHTML = myObj.STEPPER_speedZ;

    document.getElementById('STEPPER_absolueX').innerHTML = myObj.STEPPER_absolueX;
    document.getElementById('STEPPER_relativeX').innerHTML = myObj.STEPPER_relativeX;
    document.getElementById('STEPPER_absolueY').innerHTML = myObj.STEPPER_absolueY;
    document.getElementById('STEPPER_relativeY').innerHTML = myObj.STEPPER_relativeY;
    document.getElementById('STEPPER_absolueZ').innerHTML = myObj.STEPPER_absolueZ;
    document.getElementById('STEPPER_relativeZ').innerHTML = myObj.STEPPER_relativeZ;

    var stepper_xOff = myObj.STEPPER_off;

    if (stepper_xOff) {
      document.getElementById("Work").classList.add("Red_X");
      document.getElementById("Work").classList.remove("Green_X");

    }

  }, false);

}