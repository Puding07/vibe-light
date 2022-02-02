var colorPicker = new iro.ColorPicker("#picker", {
  // Set the size of the color picker
  width: 320,
  // Set the initial color to pure red
  color: "#fff",
});

var on = document.querySelector("#on");
var off = document.querySelector("#off");

const url = "192.168.0.135";

var rgb = {
  r: "254",
  g: "254",
  b: "254",
};

const options = {
  method: "GET",
  mode: "no-cors",
  cache: "no-cache",
  credentials: "same-origin",
};

var lastMove = 0;

async function colorHandler(color) {
  //alert(color.rgb);

  rgb = color.rgb;
  if (Date.now() - lastMove > 1500) {
    const response = await fetch(
      `http://${url}/rgb?r=${rgb.r}&g=${rgb.g}&b=${rgb.b}`,
      options
    );
    lastMove = Date.now();
    console.log(response.json());
  }
}

async function onHandler() {
  //alert("on");

  const response = await fetch(`http://${url}/on`, options);

  console.log(response.json());
}

async function offHandler() {
  //alert("off");

  const response = await fetch(`http://${url}/off`, options);

  console.log(response.json());
}

colorPicker.on(["color:change"], colorHandler);

on.addEventListener("click", () => colorHandler({ rgb }));
on.addEventListener("touchstart", () => colorHandler({ rgb }));

off.addEventListener("click", offHandler);
off.addEventListener("touchstart", offHandler);
