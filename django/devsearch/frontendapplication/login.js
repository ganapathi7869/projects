
let form = document.getElementById('login-form')
// console.log(form)
form.addEventListener('submit', (e) => {
    e.preventDefault()
    let formdata = {
        'username' : form.username.value,
        'password' : form.password.value
    }
    // console.log(formdata)
    fetch('http://127.0.0.1:8000/api/users/token/',{
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body:JSON.stringify(formdata)
    })
    .then(response => response.json())
    .then(jsondata => {
        if(jsondata.access){
            window.localStorage.setItem('token',jsondata.access)
        }else{
            alert('Username or Password is in-correct')
        }
        window.location = 'file:///E:/projects/django/frontendapplication/projects.html'
    })
})

