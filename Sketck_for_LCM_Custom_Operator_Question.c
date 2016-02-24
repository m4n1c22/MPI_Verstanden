/** Define a custom LCM operator method which can be used in the reduce method of MPI... 
    Lets assume we have a method LCM(val1,val2) which is called in normal/single process execution which calculates
    the result of LCM of two numbers and returns. Now we know that LCM operation is commutative and associative.
    Therefore we can define it as a MPI_User_function in which we can provide the method call for LCM internally.
    And then, use the above MPI_User_function with the new operator method to create a custom operator which can 
    eventually be used with Reduce method.
*/

#define DIM_VALS_ARR 5

/** Method for MPI_Op create method*/
void LCM_Op_function(int *invec,int *intoutvec,int *len,MPI_Datatype datatype) {
  int i;
  for(i=0;i<*len;i++) {
    inoutvec[i] = LCM(inoutvec[i],invec[i]);
  }
}

/** Method used by the root process to calculate the final lcm obtained from all the processes.*/
int lcm_local_operation(int *vals_array,int len) {
   int lcm_res = vals_array[0];
   for(i=1;i<len;i++) {
        lcm_res = LCM(lcm_res,vals_array[i]);
   }
   return lcm_res;
}

main() {
  
  int vals_array[DIM_VALS_ARR],resultant_lcm,res[DIM_VALS_ARR];
  int root = 0;

  ...//Initialize vals_array

  MPI_Op LCM_OP;
  MPI_Op_create((MPI_User_function*)LCM_Op_function,1,&LCM_OP);
  
  /** 
    Alternative implementation to local operation after reduce will be use MPI_Reduce_Scatter_Block instead of MPI_Reduce
    Then using MPI_Reduce on the scatter values.
  */
  
  /**Possible Implementation with local operation */
  MPI_Reduce(vals_array,res,DIM_VALS_ARR,MPI_INT,LCM_OP,root,MPI_COMM_WORLD);
  result = lcm_local_operation(res,DIM_VALS);
  MPI_BCAST(result,1,MPI_INT,root,MPI_COMM_WORLD);
  
  /** Possible Implementation with Reduce_Scatter method*/
  MPI_Reduce_scatter_block(vals_array,res,1,MPI_INT,LCM_OP,root,MPI_COMM_WORLD);
  MPI_Reduce(res,resultant_lcm,1,MPI_INT,LCM_OP,root,MPI_COMM_WORLD);
  MPI_Op_Free(LCM_OP);
  
}
