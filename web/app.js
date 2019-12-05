//Credenciais Firebase

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

    // Cria os listeners dos dados no firebase
    let umidRef = db.ref("umidade");

    // Registra as funções que atualizam os gráficos e dados atuais da telemetria
    umidRef.on(
      "value",
      onNewData("currentUmid", "umidLineChart", "Umidade", "%")
    );

    document.getElementById("myDiv").style.display = "block";
  }, 1500);
}

// Retorna uma função que de acordo com as mudanças dos dados
// Atualiza o valor atual do elemento, com a metrica passada (currentValueEl e metric)
// e monta o gráfico com os dados e descrição do tipo de dados (chartEl, label)
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

// Constroi um gráfico de linha no elemento (el) com a descrição (label) e os
// dados passados (data)
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
