#include <stdio.h>
#include <stdlib.h>

#define ERROR -1

// runtime inputs
typedef struct RUNTIME_INPUTS_T {
    long long   t_compiler_ns;        // time it takes for a compiler to optimize
    int         qc_s;                 // quantum circuit stage
    int         n_max_gate_per_stage; // number of maximum quantum gate per stage
    long long*  t_qubit_swap_ns;      // total time for qubit swap per qc stage
    long long** t_gate_ns;            // execution time to execute a gate in qc stage
    long long   t_measurement_ns;     // qubit measurement time
    long long   t_qec_detection_ns;   // QEC error detection time
    long long   t_qec_repair_ns;      // QEC repair time
    long long   t_vqa_classical_ns;   // time for VQA classical part
    long long   n_samples_per_iteration;  // number of samples per iteration
    long long   n_iteration;              // number of iteration
    long long   t_vqa_final_ns;       // time to execute a final data analysis

}runtime_inputs_t;

//----------------------------------------
// Name: return_max
// Returns the maximum value among a given arr
//----------------------------------------
long long return_max(long long* arr, int arr_size)
{
    long long arr_max;

    // error if arr_size isn't big enough
    if(arr_size <= 0)
    {
        printf("ERROR @ function return_max : small arr_size\n");
        return ERROR;
    }

    // find out the maximum value among arr
    arr_max = arr[0];
    for(int i=1; i<arr_size; i++)
    {
        if(arr_max < arr[i]) 
            arr_max = arr[i];
    }

    return arr_max;
}

//----------------------------------------
// Name: vqa_runtime
// Return the total run time to execute VQA with the given inputs
//----------------------------------------
long long vqa_runtime(runtime_inputs_t inputs)
{
    int s; // iterator
    long long t_vqa_quantum_ns = 0;
    long long t_hybrid_ns = 0;

    // Calculate t_vqa_quantum
    for(int s=0; s<inputs.qc_s; s++)
    {
        long long t_max_gate_ns = return_max(inputs.t_gate_ns[s], inputs.n_max_gate_per_stage);
        if(t_max_gate_ns == ERROR) 
            return ERROR;

        t_vqa_quantum_ns += (inputs.t_qubit_swap_ns[s] + t_max_gate_ns);
    }
    t_vqa_quantum_ns += inputs.qc_s*(inputs.t_measurement_ns + inputs.t_qec_detection_ns + inputs.t_qec_repair_ns);

    // Calculate an execution time of a hybrid loop
    t_hybrid_ns = (t_vqa_quantum_ns+inputs.t_vqa_classical_ns)*inputs.n_samples_per_iteration*inputs.n_iteration;


    // return total simulation time
    return (inputs.t_compiler_ns + t_hybrid_ns + inputs.t_vqa_final_ns);
}

int main()
{
    int i, j; // iterator
    long long   t_sim = 0;                // result value
    runtime_inputs_t inputs = {           // initialize input parameters to random values
        .t_compiler_ns = 1,        
        .qc_s = 2,                 
        .n_max_gate_per_stage = 3, 
        .t_qubit_swap_ns = malloc(inputs.qc_s * sizeof(long long)),          
        .t_gate_ns = malloc(inputs.qc_s * sizeof(long long*)),                
        .t_measurement_ns = 4,     
        .t_qec_detection_ns = 5,   
        .t_qec_repair_ns = 6,      
        .t_vqa_classical_ns = 7,   
        .n_samples_per_iteration = 8,
        .n_iteration = 9,            
        .t_vqa_final_ns = 10      
    };

    for(i=0; i<inputs.qc_s; i++)
        inputs.t_gate_ns[i] = malloc(inputs.n_max_gate_per_stage * sizeof(long long));

    for(i=0; i<inputs.qc_s; i++)
    {
        inputs.t_qubit_swap_ns[i] = (i+1)*100;

        for(j=0; j<inputs.n_max_gate_per_stage; j++)
            inputs.t_gate_ns[i][j] = (i+1)*10000 + (j+1)*1000;
    }

    printf("VQA runtime estimation\n");


    // Get VQA runtime value and print the result.
    t_sim = vqa_runtime(inputs);

    if(t_sim == ERROR) printf("ERROR occurred. Exit.\n");
    else printf("Total estimated simulation time for the given VQA = %lld ns", t_sim);


    // free memory
    free(inputs.t_qubit_swap_ns);
    free(inputs.t_gate_ns);
    return 0;
}