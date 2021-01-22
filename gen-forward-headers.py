#!/usr/bin/env python
from autogen.autogen import autogen

project = "KDReports"
version = "1.9.1"
subprojects = ["KDReports"]
prefixed = True

autogen(project, version, subprojects, prefixed, steps = ["generate-forward-headers"], policyVersion = 2)
