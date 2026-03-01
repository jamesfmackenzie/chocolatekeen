
// changes font size in p tag sections- default is 12 pt
// usage
// <a href="javascript:decreaseFontSize();">-</a> 
// <a href="javascript:increaseFontSize();">+</a>
var fontsizemin=8;
var fontsizemax=18;
function increaseFontSize() {
   var p = document.getElementsByTagName('p');
   for(i=0;i<p.length;i++) {
      if(p[i].style.fontSize) {
         var s = parseInt(p[i].style.fontSize.replace("px",""));
      } else {
         var s = 14;
      }
      if(s!=fontsizemax) {
         s += 2;
      }
      p[i].style.fontSize = s+"px"
   }
}
function decreaseFontSize() {
   var p = document.getElementsByTagName('p');
   for(i=0;i<p.length;i++) {
      if(p[i].style.fontSize) {
         var s = parseInt(p[i].style.fontSize.replace("px",""));
      } else {
         var s = 12;
      }
      if(s!=fontsizemin) {
         s -= 2;
      }
      p[i].style.fontSize = s+"px"
   }   
}
