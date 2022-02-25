# https://drawsql.app/ganapathi7869/diagrams/dev-connect
from enum import unique
from django.db import models
# Create your models here.
import uuid
from users.models import Profile

class Project(models.Model):
    owner = models.ForeignKey(Profile,on_delete=models.SET_NULL,null=True,blank=True)
    title = models.CharField(max_length=200)
    description = models.TextField(null=True,blank=True)
    featured_image = models.ImageField(null=True,blank=True, default='default.jpg')
    demo_link = models.CharField(max_length=2000, null=True,blank=True)
    source_link = models.CharField(max_length=2000, null=True,blank=True)
    tags = models.ManyToManyField('Tag',blank=True) #anotherway: tag = models.ManyToManyField(Tag,null=True)
    vote_total = models.IntegerField(default=0,null=True,blank=True)
    vote_ratio = models.IntegerField(default=0,null=True,blank=True)
    created = models.DateTimeField(auto_now_add=True)
    id = models.UUIDField(default=uuid.uuid4,unique=True,primary_key=True,editable=False)

    def __str__(self):
        return self.title

    class Meta:
        ordering = ['-vote_ratio','-vote_total','title']      #desc: ['-created']

    @property
    def getreviewers(self):
        reviewers = self.review_set.all().values_list('owner__id',flat=True)
        return reviewers

    def updatevotes(self):
        self.vote_total = self.review_set.all().count()
        positive_votes = self.review_set.filter(value='up').count()
        self.vote_ratio = positive_votes / self.vote_total * 100 if self.vote_total else 0
        self.save()

class Review(models.Model):
    VOTE_TYPE = (
        ('up','Up Vote'),
        ('down','Down Vote')
    )

    owner = models.ForeignKey(Profile,on_delete=models.CASCADE,null=True)
    project = models.ForeignKey(Project,on_delete=models.CASCADE)
    body = models.TextField(null=True,blank=True)
    value = models.CharField(max_length=200,choices=VOTE_TYPE)
    created = models.DateTimeField(auto_now_add=True)
    id = models.UUIDField(default=uuid.uuid4,unique=True,primary_key=True,editable=False)

    class Meta:
        unique_together = [['owner','project']]

    def __str__(self):
        return self.value

class Tag(models.Model):
    name = models.CharField(max_length=200)
    created = models.DateTimeField(auto_now_add=True)
    id = models.UUIDField(default=uuid.uuid4,unique=True,primary_key=True,editable=False)
 
    def __str__(self):
        return self.name
