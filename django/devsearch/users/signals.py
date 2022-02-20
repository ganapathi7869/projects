from django.db.models.signals import post_save,post_delete
from django.contrib.auth.models import User
from .models import Profile

def createProfile(sender,instance,created,**kwargs):
    if created:
        user=instance
        prof=Profile.objects.create(
            user = user,
            username = user.username,
            email = user.email,
            name = user.first_name
        )

def deleteUser(sender,instance,**kwargs):
    prof = instance
    try:    prof.user.delete()
    except: pass

def updateUser(sender,instance,created,**kwargs):
    prof = instance
    if created == False:
        user = prof.user
        user.first_name = prof.name
        user.username = prof.username
        user.email = prof.email
        user.save()

post_save.connect(createProfile,sender=User)
post_delete.connect(deleteUser,sender=Profile)
post_save.connect(updateUser,sender=Profile)