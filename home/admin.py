from django.contrib import admin
from . import models
import pytz

# Register your models here.
admin.site.register(models.Course)

from .models import Course

