# Generated by Django 4.0.1 on 2022-02-21 10:50

from django.db import migrations


class Migration(migrations.Migration):

    dependencies = [
        ('users', '0003_profile_location_skill'),
    ]

    operations = [
        migrations.AlterModelOptions(
            name='profile',
            options={'ordering': ['created']},
        ),
    ]
