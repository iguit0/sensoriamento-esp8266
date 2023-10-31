// Firebase Credentials

const firebaseConfig = {
  apiKey: "apiKey",
  authDomain: "authDomain",
  databaseURL: "databaseURL",
  projectId: "projectId",
  storageBucket: "storageBucket",
  messagingSenderId: "messagingSenderId",
  appId: "appId",
  measurementId: "measurementId"
};

firebase.initializeApp(firebaseConfig);
firebase.analytics();

function loadData() {
  setTimeout(() => {
    document.getElementById("loader").style.display = "none";

    let db = firebase.database();

    // Instantiate Firebase Listener
    let umidRef = db.ref("umidade");

    // Records functions that update current telemetry graphs and data
    umidRef.on(
      "value",
      onNewData("currentUmid", "umidLineChart", "Umidade", "%")
    );

    document.getElementById("myDiv").style.display = "block";
  }, 1500);
}

// Returns a function that according to data changes
// Update the current value of the element, with the passed metric (currentValueEl and metric)
// and assembles the chart with the data and description of the data type (chartEl, label)
function onNewData(currentValueEl, chartEl, label, metric) {
  return function(snapshot) {
    var readings = snapshot.val();

    if (readings) {
      var currentValue;
      var data = [];
      for (var key in readings) {
        currentValue = readings[key];
        data.push(currentValue);
      }

      document.getElementById(currentValueEl).innerText =
        currentValue + " " + metric;
      buildLineChart(chartEl, label, data);
    }
  };
}

// Builds a line graph on the element (el) with the description (label) and the past data (date)
function buildLineChart(el, label, data) {
  var elNode = document.getElementById(el);
  new Chart(elNode, {
    type: "line",
    data: {
      labels: new Array(data.length).fill(""),
      datasets: [
        {
          label: label,
          data: data,
          borderWidth: 1,
          fill: false,
          spanGaps: false,
          lineTension: 0.1,
          backgroundColor: "#F9A825",
          borderColor: "#F9A825"
        }
      ]
    }
  });
}
