import sys
import json
import matplotlib.pyplot as plt
import numpy as np


# This should be the input json file. 
input_file = sys.argv[1];

# Opening JSON file
f = open(input_file,)
  
# returns JSON object as a dictionary
data = json.load(f)

xvalues = data["xvalues"];
series = data["series"];

nolegend = 0
if (len(series) < 2):
   nolegend = 1

for label in series:
   yvalues = series[label]
   if (nolegend == 0):
      plt.plot(xvalues,yvalues,label = label)
   else:
      plt.plot(xvalues,yvalues)

if (nolegend == 0):
   plt.legend()

if ('xlabel' in data):
   plt.xlabel(data["xlabel"])

if ('ylabel' in data):
   plt.ylabel(data["ylabel"])

if ('title' in data):
   plt.title(data["title"])

plt.savefig(data["output_file"],dpi=300)

