from django.forms import CheckboxSelectMultiple, ModelForm
from .models import Project
from django import forms

class ProjectForm(ModelForm):
    class Meta:
        model = Project
        fields = ['title' ,'featured_image','description','demo_link','source_link','tags'] #anotherway: fields = '__all__'
        widgets = {
            'tags': forms.CheckboxSelectMultiple(),
        }

    def __init__(self,*args,**kwargs):
        super(ProjectForm,self).__init__(*args,**kwargs)
        for fieldname,field in self.fields.items():
            field.widget.attrs.update({'class':'input'})
