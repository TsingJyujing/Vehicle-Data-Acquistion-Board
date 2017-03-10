import glob
import re
import os
if __name__=="__main__":
    bins = glob.glob('*.bin')
    for binfn in bins:
        fn_type = re.findall('\.(mpu|gps|can)\.', binfn)
        if len(fn_type)>0:
            fn_type = fn_type[-1]
            fn_out = binfn.replace('bin','csv')
            os.system('bin2csv %s %s csv/%s' % (fn_type, binfn, fn_out))