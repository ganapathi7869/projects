# Generated by Django 4.0.1 on 2022-01-25 11:50

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('projects', '0003_alter_project_votes_ratio_alter_project_votes_total'),
    ]

    operations = [
        migrations.RenameField(
            model_name='project',
            old_name='votes_ratio',
            new_name='vote_ratio',
        ),
        migrations.RenameField(
            model_name='project',
            old_name='votes_total',
            new_name='vote_total',
        ),
    ]
