//On document start
$(document).ready(function() {
   var power = false;
   $('#power').bootstrapToggle('off')
   $("#brightness").prop('disabled', true);
   $("#pattern").prop('disabled', true);

  //Set backgroundColor
  document.body.style.backgroundColor = "rgba(111,110,110,0.4)";

  //Power Switch
  $('#power').change(function(){
    power = $(this).prop('checked');
    if(power === false) {
      document.body.style.backgroundColor = "rgba(111,110,110,0.4)";
      $("#brightness").prop('disabled', true);
      $("#pattern").prop('disabled', true);
      $.post("off")
    } else {
      $("#brightness").prop('disabled', false);
      $("#pattern").prop('disabled', false);
      $.post("on")
    }
  })

  $('.link').click(function(e) {
    if(power === false) {
      e.preventDefault();
    } else {
      e.preventDefault();
      var value = $(this).attr("href");
      $.post(value);
    }
  });

  //Init Color Picker
  var colorPicker = new iro.ColorPicker("#picker", {
    width: 320,
    color: "#fff",
    borderWidth: 2,
    layout: [
    {
      component: iro.ui.Wheel,
    },
  ]
  });

  //Send color to console after release
  colorPicker.on('input:end', function(color) {
    if(power === false) {
      return;
    } else {
      var choice = {};
      choice.red = color.red;
      choice.green = color.green;
      choice.blue = color.blue;
      console.log(choice);
      $.post("color?red=" + choice.red + "&green=" + choice.green + "&blue=" + choice.blue);
    }
  });

  //Change backgroundColor while using picker
  colorPicker.on('color:change', function(color) {
    if(power === false) {
      return;
    } else {
      document.body.style.backgroundColor = "" + color.rgbaString;
    }
  });

});

//Function to setBrightness and post
function setBrightness(slideAmount) {
  brightness = slideAmount;
  console.log(brightness);
  $.post("brightness?amount=" + brightness);
}

//Function to change pattern and post
function selectPattern() {
  var selObj = document.getElementById("pattern");
  var pattern = selObj.options[selObj.selectedIndex].value;
  console.log(pattern);
  $.post("pattern?choice=" + pattern);
  document.getElementById("pattern").value = -1;
}
