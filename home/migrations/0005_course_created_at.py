# Generated by Django 4.2.7 on 2023-12-11 13:24

from django.db import migrations, models
import django.utils.timezone


class Migration(migrations.Migration):

    dependencies = [
        ('home', '0004_remove_course_created_at'),
    ]

    operations = [
        migrations.AddField(
            model_name='course',
            name='created_at',
            field=models.DateTimeField(default=django.utils.timezone.now),
        ),
    ]
