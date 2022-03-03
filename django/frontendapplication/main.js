let loginbtn = document.getElementById('loginbtn')
let logoutbtn = document.getElementById('logoutbtn')
if(window.localStorage.getItem('token')){
    loginbtn.remove()
}else{
    logoutbtn.remove()
}

logoutbtn.addEventListener('click', (e) => {
    e.preventDefault()
    window.localStorage.removeItem('token')
    window.location = 'file:///E:/projects/django/frontendapplication/projects.html'
})

let projectsurl = "http://127.0.0.1:8000/api/projects/"
let getprojects = () => {
    fetch(projectsurl)
    .then(response => response.json())
    .then(jsondata => {
        buildprojects(jsondata)
    })
}

//custom attributes for html tag in below function: data-<custom variable>="<value>" 
let buildprojects = (jsondata) => {
    let projectswrapper = document.getElementById('projects--wrapper')
    projectswrapper.innerHTML = ''
    for(let i=0;i<jsondata.length;i++){
        let proj = jsondata[i]
        projectswrapper.innerHTML += `
            <div class="project--card">
                <img src="http://127.0.0.1:8000${proj.featured_image}">
                <div>
                    <div class="project--header">
                        <h3>${proj.title}</h3>
                        <strong class="vote--option" data-votevalue="up" data-projectid="${proj.id}">&#43;</strong>
                        <strong class="vote--option" data-votevalue="down" data-projectid="${proj.id}">&#8722;</strong>
                    </div>
                    <i>${proj.vote_ratio}% Positive feedback </i>
                    <p>${proj.description.substring(0,150)}</p>
                </div>
            </div>
        `
    }
    addvoteevents()
}

let addvoteevents = () => {
    let votebuttons = document.getElementsByClassName('vote--option')
    for(let i=0; i<votebuttons.length; i++){
        votebuttons[i].addEventListener('click', (e) => {
            let token = window.localStorage.getItem('token')
            fetch(`http://127.0.0.1:8000/api/projects/${e.target.dataset.projectid}/vote/`,{
                method:'POST',
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${token}`
                },
                body: JSON.stringify({'value':`${e.target.dataset.votevalue}`})
            })
            .then(response => getprojects())
        })
    }
}

getprojects()
