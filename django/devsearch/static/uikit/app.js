// Invoke Functions Call on Document Loaded
document.addEventListener('DOMContentLoaded', function(){
  hljs.highlightAll();
});

let alertsWrapper = document.querySelectorAll('.alert')
let alertsClose = document.querySelectorAll('.alert__close')

for(let i=0;i<alertsClose.length;i++){
  alertsClose[i].addEventListener('click', () => {
    alertsWrapper[i].style.display = 'none';
  })
}