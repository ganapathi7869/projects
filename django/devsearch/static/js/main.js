
let tagelements = document.getElementsByClassName('projecttag')

for(let i=0;i<tagelements.length;i++){
    tagelements[i].addEventListener('click',(e)=>{
        tagid = e.target.dataset.tagid
        projectid = e.target.dataset.projectid

        fetch('http://127.0.0.1:8000/api/remove-tag/',{
            method: 'DELETE',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({'projectid':projectid,'tagid':tagid})
        })
        .then(response => {
            e.target.remove()
        })
    })
}