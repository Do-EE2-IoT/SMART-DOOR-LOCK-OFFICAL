from django.db import models
from django.utils import timezone

class Course(models.Model):
    content = models.CharField(max_length=200)
    vietnam_time = models.DateTimeField(default=timezone.now)


    