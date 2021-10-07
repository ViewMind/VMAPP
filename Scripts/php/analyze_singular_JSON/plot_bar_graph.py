import sys
import json
import matplotlib.pyplot as plt
import numpy as np

# Reducing font size
plt.rc('xtick', labelsize=5)

# This should be the input json file. 
input_file = sys.argv[1];

# Opening JSON file
f = open(input_file,)
  
# returns JSON object as a dictionary
data = json.load(f)

xvalues = data["xvalues"];
series = data["series"];

x = np.arange(len(xvalues))  # the label locations
width = 0.35  # the width of the bars

fig, ax = plt.subplots()

### THIS IS HORRIBLE. Figure out how to do an N Bar Graph and Generalize it, for GOD's Sake. (When you have time)
if (len(series) == 2):
   first = 1
   for label in series:
      yvalues = series[label]
      if (first == 1):      
         ax.bar(x+width/2,yvalues,width,label = label)
         first = 0
      else:
         ax.bar(x-width/2,yvalues,width,label = label)
elif (len(series) == 1):
   for label in series:
      yvalues = series[label]
      ax.bar(x,yvalues,width,label = label)
else:
   print("ERROR on BarPlot: Only up to 2 Bar Graph is Supported");
   exit()

# Setting the values below the bars.
ax.set_xticks(x)
ax.set_xticklabels(xvalues)

# Not really sure what this does. But it makes everything look pretty. 
fig.tight_layout()
ax.legend()

if ('xlabel' in data):
   plt.xlabel(data["xlabel"])

if ('ylabel' in data):
   plt.ylabel(data["ylabel"])

if ('title' in data):
   plt.title(data["title"])

plt.savefig(data["output_file"],dpi=300,bbox_inches = "tight")

