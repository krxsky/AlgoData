def compress(h):
    result = {}       #dict to store result
    def helper(h, acc):
        #acc keeps track of the keys so far
        if isinstance(h, dict):  #reduction step
            #list comprehension is used as syntactical sugar
            [helper(v,acc+"_"+k) for k,v in h.items()] 
        else:        #base case
            result[acc[1:]] = h  #add to the result dict
    helper(h, "")
    return result
