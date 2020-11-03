import random;
import copy;

MAX_TRIAGLE_TYPE_IN_ROW_OR_COL = 3;
NUMBER_OF_YES_TRIAL_CYCLES     = 5;
NUMBER_OF_NO_TRIALS            = 30;


UNIQUE_TYPE = "L"
SEQUENCE = """22 14 13 15 18
              11 5  4  6  12
              2  1  X  3  20
              8  7  10 9  19
              24 21 23 16 17""";
SEQUENCE_LENGTH = 24;
FINAL_SET_LENGTH = SEQUENCE_LENGTH*NUMBER_OF_YES_TRIAL_CYCLES + NUMBER_OF_NO_TRIALS;

### Function for resetting the row column-counter 
def getZeroCounters():
    
    counter = {};
    
    counter["D"] = {};
    counter["D"]["row"] = [0,0,0,0,0];
    counter["D"]["col"] = [0,0,0,0,0];
    
    counter["U"] = {};
    counter["U"]["row"] = [0,0,0,0,0];
    counter["U"]["col"] = [0,0,0,0,0];
    
    
    counter["L"] = {};
    counter["L"]["row"] = [0,0,0,0,0];
    counter["L"]["col"] = [0,0,0,0,0];
    
    
    counter["R"] = {};
    counter["R"]["row"] = [0,0,0,0,0];
    counter["R"]["col"] = [0,0,0,0,0];

    return counter;
   
def generateATrial(trial_type,unique_triangle_row,unique_triangle_col):
   trial = "";
   counter = getZeroCounters();
   
   for row in range(0,5):
       trial_row = "";
       
       #print("Row " + str(row));

       for col in range(0,5):  
        
           #print("   Col " + str(col));
           
           # The center must always have the cross. 
           if ((row == 2) and (col == 2)) :
               trial_row = trial_row + "+ ";
               continue;
              
           if ((row == unique_triangle_row) and (col == unique_triangle_col)) :
               trial_row = trial_row + trial_type + " ";
               continue;
                   
           possible_triangles = [];
           for triangle_type in counter:
            
               #print("      " + triangle_type + " -> RC: " + str(counter[triangle_type]["row"][row]) + ". CC: " + str(counter[triangle_type]["col"][col]) );
               
               # The triangle of the trial type can appear ONLY once so it's not a possible for random choice. 
               if (trial_type == triangle_type):
                  continue;
               
               if (counter[triangle_type]["row"][row] >= MAX_TRIAGLE_TYPE_IN_ROW_OR_COL):
                  continue;
                   
               if (counter[triangle_type]["col"][col] >= MAX_TRIAGLE_TYPE_IN_ROW_OR_COL):
                  continue;
               
               #print("Appending triangle " + triangle_type);
               possible_triangles.append(triangle_type);
           
           triangle = random.choice(possible_triangles);
           trial_row = trial_row + triangle + " ";
           #print("         Selected: " + triangle);
           #print(counter);
           counter[triangle]["row"][row] = counter[triangle]["row"][row] + 1;
           counter[triangle]["col"][col] = counter[triangle]["col"][col] + 1;
           #print(counter);
       
       trial = trial + "\n" + trial_row;
       #print(trial);
           
   #print(trial);
   return trial;


# Generating the coordinate sequence based on on locations
sequence_array =  [ [] for number in range(SEQUENCE_LENGTH)];

sequence_lines = SEQUENCE.split("\n");
row_index = 0;
for line in sequence_lines:
   line = line.strip();
   numbers = line.split(" ");
   col_index = 0;
   #print(numbers);
   for n in numbers:
      n = n.strip();
      if (n == ""):
         continue; 
      if (n == "X"):
         col_index = col_index + 1;       
         continue;
      pos = int(n) - 1; 
      sequence_array[pos] = [row_index,col_index];      
      col_index = col_index + 1;
   row_index = row_index + 1;

# print(sequence_array);
#temp = copy.deepcopy(sequence_array);

# Repeating the sequence the number of required times.
final_sequence_array = [];

for i in range(NUMBER_OF_YES_TRIAL_CYCLES):
   final_sequence_array = final_sequence_array + sequence_array;

L = len(final_sequence_array);
#print(L);

# Generating the ineces for the point of insertion of NO type trials in the final_sequence_array.
n_type_locations = [];
where_to_pick = list(range(L));
#print(where_to_pick);
for i in range(NUMBER_OF_NO_TRIALS):
   index = random.randint(0,len(where_to_pick)-1);   
   n_type_locations.append(where_to_pick[index]);
   del where_to_pick[index];
   
n_type_locations.sort();
print(n_type_locations);

# Inserting into the final sequence. 
for i in n_type_locations:
   final_sequence_array.insert(i,[]);
   
L = len(final_sequence_array);
#print(L);
#print(final_sequence_array);

# Generating the output text file 
h = open("perception_study.dat","w")
h.write("v1\n");

for trial_id in range(L):
   trial_name = str(trial_id);
   while (len(trial_name) < 3):
      trial_name = "0" + trial_name;
   
   trial_loc = final_sequence_array[trial_id];
   #print(trial_loc);
   trial_desc = "";
   trial_type = "N";
   row_loc = -1;
   col_loc = -1;
   if (len(trial_loc) == 2):
      trial_type =  UNIQUE_TYPE;
      row_loc = trial_loc[0];
      col_loc = trial_loc[1];
      
   trial_desc = generateATrial(trial_type,row_loc,col_loc);   
      
   h.write(trial_name + " " + trial_type + " " + str(row_loc) + " " + str(col_loc));
   h.write(trial_desc + "\n\n");

h.close();
print("Finished");







