for(var i = 0; i < 16;i++) {
  $('#'+i.toString()).on('click', function () {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      
      if (this.readyState == 4 && this.status == 200) {
        Object.entries(JSON.parse(xhr.responseText)).forEach(([key, value]) => {
          $(key).attr('mode',value)
        });
      }
    }
    xhr.open('POST', '/led/'+$(this).attr('id')+'/'+$($(this).children()).attr('mode'))
    xhr.send()
  })
}
var xhr = new XMLHttpRequest();
xhr.onreadystatechange = function() {
  if (this.readyState == 4 && this.status == 200) {
    Object.entries(JSON.parse(xhr.responseText)).forEach(([key, value]) => {
      $(key).attr('mode',value)
    });
  }
}
xhr.open('GET','/data/')
xhr.send();
for(var i = 1; i < 6;i++) {
  $('#anim'+i.toString()).on('click', function () {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', '/animation/'+$(this).attr('id')+'/')
    xhr.send()
  })
}
