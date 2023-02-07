import numpy as np

def choice(probs):
    ''' 
    Returns the position of a random choice given an array of probabilities.
    If the input is an array of shape (N,) then it returns an index, and if 
    the input shape is (N,M) then it returns the row and column of the choice.
    '''
    if len(probs.shape) == 1:           # Input array is 1D
        N = len(probs)    
        probs_acc = np.zeros((N,))      # probs_acc is the cummulative sum of probs
        probs_acc[0] = probs[0]
        for i in range(1, N):
            probs_acc[i] = probs_acc[i-1] + probs[i]
        random_value = np.random.rand(1)
        i = 0
        while probs_acc[i] < random_value:
            i = i + 1
            if i == len(probs_acc)-1:   # This is in case probs_acc[-1] < 1
                break
        return i
    elif len(probs.shape) == 2:         # Input array is 2D
        rows, cols = probs.shape
        index = choice(probs.flatten()) # flatten() converts 2D arrays to 1D
        r = index // cols
        c = index - r*cols
        return r, c

def generate_sequence(counts_):
    '''
    Given a 1D or 2D input array where each element is an integer that 
    represents the number of times that position must be chosen, this function 
    returns a sequence of random choices according to the input. Each element 
    in the sequence is the position (index for 1D and (row,col) for 2D arrays) 
    of the chosen array element.
    '''
    output = []
    counts = counts_.copy()                      # To avoid modifying original array
    if len(counts.shape) == 1:                   # Input array is 1D
        N = len(counts)
        while np.sum(counts) > 0:
            probs = np.zeros((N,))
            counts_sum = np.sum(counts)
            for i in range(N):
                probs[i] = counts[i]/counts_sum
            index = choice(probs)
            counts[index] = counts[index] - 1
            output.append(index)
    elif len(counts.shape) == 2:                 # Input array is 2D
        rows, cols = counts.shape    
        while np.sum(counts) > 0:
            probs = np.zeros((rows, cols))
            counts_sum = np.sum(counts)
            for r in range(rows):
                for c in range(cols):
                    probs[r,c] = counts[r,c]/counts_sum
            row, col = choice(probs)
            counts[row, col] = counts[row, col] - 1
            output.append((row, col))
    return output

def normalize(array):
    '''
    Linearly scales input array values so that the min and max values are 0 and 1
    '''
    rows, cols = array.shape
    max_value = array.max()
    min_value = array.min()
    for row in range(rows):
        for col in range(cols):
            array[row, col] = (array[row, col] - min_value)/(max_value - min_value)
    return array

def generate_sequence_conditional(counts_, sequence, min_distance, target_mean, eps=0.1):
    '''
    This function returns a sequence of random choices like in generate_sequence, 
    but with some restrictions. The first restriction is that, given an input
    sequence with the same length as the expected output sequence, the euclidean
    distance between elements in the same sequence position from both sequences 
    (input and generated) must be greater or equal than min_distance. The second 
    restriction is that the mean of those distances between sequences elements
    must be close to a target mean (it is a "soft" restriction). Finally, in 
    order to always accomplish the min_distance rule, the counts rule (how many
    times an element must be chosen) is relaxed so that some elements may differ 
    in the number of times they are chosen. This difference is minimum and only
    when its required.
    '''
    mean = 0
    output = []
    distances = []
    N = len(sequence)
    counts = counts_.copy()
    rows, cols = counts.shape
    for i in range(N):
        y, x = sequence[i]
        
        # Apply distance rule by making zero the counter of positions closer 
        # than min_distance
        distance = np.zeros((rows, cols))
        counts_masked = counts.copy()
        for r in range(rows):
            for c in range(cols):
                distance[r,c] = np.sqrt((c - x)**2 + (r - y)**2)
                if distance[r,c] < min_distance:
                    counts_masked[r,c] = 0
                    
        # In case that not all counts were zeroed, scale probabilities so it is
        # more likely that a value that makes the mean closer to target_mean 
        # will be chosen
        if np.sum(counts_masked) > 0:
            obj_value = target_mean*i - mean*(i-1)
            for r in range(rows):
                for c in range(cols):
                    scale = 1/(eps + np.abs(distance[r,c] - obj_value))
                    counts_masked[r,c] = counts_masked[r,c]*scale
        
        # If all remaining counts were zeroed (it happens at the end of the 
        # sequence or when min_distance is high), then make all positions 
        # equiprobable
        else:
            counts_masked = np.ones((rows, cols))
            
        counts_masked_sum = counts_masked.sum()
        probs = np.zeros((rows, cols))
        for r in range(rows):
            for c in range(cols):
                probs[r,c] = counts_masked[r,c]/counts_masked_sum
        row, col = choice(probs)
        
        # Save current distance to calculate current mean distance
        distances.append(np.sqrt((x - col)**2 + (y - row)**2))
        mean = np.mean(distances)
        
        # This if is to consider the case when all remaining counts were zeroed
        if counts[row, col] > 0:
            counts[row, col] = counts[row, col] - 1
        output.append((row, col))
    return output

# %% Generate sequences
# 
# Sequence 1: points where the balls appear
# Sequence 2: points to where the ball moves (on the grid, without going off the grid)
# Sequence 3: points to where the ball goes when moving towards the person
# Sequence 4: points to where the person must throw the ball

rows = 5
cols = 5
times_per_hole = 3
number_of_passes = 50
minimum_distance = 2.0
mean_distance = 3.0

counts = times_per_hole*np.ones((rows, cols))
sequence_2 = generate_sequence(counts)
sequence_3 = generate_sequence(counts)
sequence_4 = generate_sequence(counts)
sequence_1 = generate_sequence_conditional(counts, sequence_2, minimum_distance, mean_distance)
N = len(sequence_1)
passes = generate_sequence(np.array([N - number_of_passes, number_of_passes]))
for i in range(N):
    if passes[i] == 0:
        sequence_1[i] = sequence_2[i]

