
let projectsurl = "http://127.0.0.1:8000/api/projects/"

let getprojects = () => {
    fetch(projectsurl)
    .then(response => response.json())
    .then(jsondata => buildprojects(jsondata))
}

let buildprojects = (jsondata) => {
    let projectswrapper = document.getElementById('projects--wrapper')
    for(let i=0;i<jsondata.length;i++){
        let proj = jsondata[i]
        projectswrapper.innerHTML += `
            <div class="project--card">
                <img src="http://127.0.0.1:8000${proj.featured_image}">
                <div>
                    <div class="project--header">
                        <h3>${proj.title}</h3>
                        <strong class="vote--option">&#43;</strong>
                        <strong class="vote--option">&#8722;</strong>
                    </div>
                    <i>${proj.vote_ratio}% Positive feedback </i>
                    <p>${proj.description.substring(0,150)}</p>
                </div>
            </div>
        `
    }
}

getprojects()
