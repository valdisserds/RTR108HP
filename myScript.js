function bulbOn()
{
document.getElementById('myImage').src='pic_bulbon.gif';
document.getElementById('demo').style.fontSize='35px';
} 

function bulbOff()

{
document.getElementById('myImage').src='pic_bulboff.gif';
document.getElementById('demo').style.fontSize='16px';
}

//creates a listener for when you press a key
window.onkeyup = keyup;

//creates a global Javascript variable
var inputTextValue;

function keyup(e) {
  //setting your input text to the global Javascript Variable for every key press
  inputTextValue = e.target.value;

  //listens for you to press the ENTER key, at which point your web address will change to the one you have input in the search box
  if (e.keyCode == 13) {
    window.location = "http://www.draugiem.lv";
  }
}


