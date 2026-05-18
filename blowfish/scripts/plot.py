import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

sns.set_theme(style="whitegrid")

try:
    df_mpi = pd.read_csv('results/mpi.csv')
    df_omp = pd.read_csv('results/omp.csv')
    df_seq = pd.read_csv('results/seq.csv')

    if df_mpi.empty or df_omp.empty or df_seq.empty:
        raise ValueError("One of the CSV files is empty. Please run all benchmark scripts first!")

    df_mpi['data_size'] = df_mpi['data_size'].astype(str).str.upper()
    df_omp['data_size'] = df_omp['data_size'].astype(str).str.upper()
    df_seq['data_size'] = df_seq['data_size'].astype(str).str.upper()

    df_mpi_avg = df_mpi.groupby(['processes', 'data_size'], as_index=False)['time'].mean()
    df_omp_avg = df_omp.groupby(['threads', 'data_size'], as_index=False)['time'].mean()
    df_seq_avg = df_seq.groupby(['data_size'], as_index=False)['time'].mean()

    df_mpi_avg = df_mpi_avg.rename(columns={'processes': 'workers'})
    df_omp_avg = df_omp_avg.rename(columns={'threads': 'workers'})

    def calculate_true_metrics(df, df_seq_baseline):
        df['speedup'] = 0.0
        df['efficiency'] = 0.0
        for size in df['data_size'].unique():
            baseline_data = df_seq_baseline[df_seq_baseline['data_size'] == size]['time'].values
            if len(baseline_data) == 0:
                raise KeyError(f"Size '{size}' found in parallel data but missing in sequential baseline (seq.csv)!")
            
            t1_pure = baseline_data[0]
            mask = df['data_size'] == size
            df.loc[mask, 'speedup'] = t1_pure / df.loc[mask, 'time']
            df.loc[mask, 'efficiency'] = df.loc[mask, 'speedup'] / df.loc[mask, 'workers']
        return df

    df_mpi_avg = calculate_true_metrics(df_mpi_avg, df_seq_avg)
    df_omp_avg = calculate_true_metrics(df_omp_avg, df_seq_avg)

    fig, axes = plt.subplots(1, 3, figsize=(22, 6))
    
    sizes = ['1MB', '5MB', '20MB']
    colors = {'1MB': 'tab:blue', '5MB': 'tab:orange', '20MB': 'tab:red'}
    
    for size in sizes:
        mpi_data = df_mpi_avg[df_mpi_avg['data_size'] == size]
        omp_data = df_omp_avg[df_omp_avg['data_size'] == size]
        
        seq_time = df_seq_avg[df_seq_avg['data_size'] == size]['time'].values[0]
        
        axes[0].axhline(y=seq_time, color=colors[size], linestyle=':', alpha=0.6, 
                        label=f'Sequential ({size})')
        
        if not mpi_data.empty:
            axes[0].plot(mpi_data['workers'], mpi_data['time'], 'o--', label=f'MPI ({size})', color=colors[size])
        if not omp_data.empty:
            axes[0].plot(omp_data['workers'], omp_data['time'], 's-', label=f'OpenMP ({size})', color=colors[size])
    
    axes[0].set_title('Execution Time Comparison', fontsize=14, fontweight='bold')
    axes[0].set_xlabel('Number of Workers (Processes / Threads)')
    axes[0].set_ylabel('Time (seconds)')
    axes[0].set_yscale('log')
    axes[0].legend()

    for size in sizes:
        mpi_data = df_mpi_avg[df_mpi_avg['data_size'] == size]
        omp_data = df_omp_avg[df_omp_avg['data_size'] == size]
        
        if not mpi_data.empty:
            axes[1].plot(mpi_data['workers'], mpi_data['speedup'], 'o--', label=f'MPI ({size})', color=colors[size])
        if not omp_data.empty:
            axes[1].plot(omp_data['workers'], omp_data['speedup'], 's-', label=f'OpenMP ({size})', color=colors[size])

    max_workers = max(df_mpi_avg['workers'].max(), df_omp_avg['workers'].max())
    axes[1].plot([1, max_workers], [1, max_workers], 'k--', alpha=0.7, label='Ideal Speedup')
    
    axes[1].set_title('Speedup Factor by Data Size', fontsize=14, fontweight='bold')
    axes[1].set_xlabel('Number of Workers')
    axes[1].set_ylabel('Speedup Ratio')
    axes[1].legend()

    for size in sizes:
        mpi_data = df_mpi_avg[df_mpi_avg['data_size'] == size]
        omp_data = df_omp_avg[df_omp_avg['data_size'] == size]
        
        if not mpi_data.empty:
            axes[2].plot(mpi_data['workers'], mpi_data['efficiency'] * 100, 'o--', label=f'MPI ({size})', color=colors[size])
        if not omp_data.empty:
            axes[2].plot(omp_data['workers'], omp_data['efficiency'] * 100, 's-', label=f'OpenMP ({size})', color=colors[size])
        
    axes[2].set_title('Parallel Efficiency (%)', fontsize=14, fontweight='bold')
    axes[2].set_xlabel('Number of Workers')
    axes[2].set_ylabel('Efficiency (%)')
    axes[2].set_ylim(0, 110)
    axes[2].legend()

    table_mpi = df_mpi_avg.pivot(index='workers', columns='data_size', values='time')
    table_omp = df_omp_avg.pivot(index='workers', columns='data_size', values='time')

    table_mpi = table_mpi.reindex(columns=sizes)
    table_omp = table_omp.reindex(columns=sizes)

    with open('results/performance_tables.md', 'w') as f_table:
        f_table.write("### MPI Execution Time (Seconds)\n\n")
        f_table.write(table_mpi.to_markdown() + "\n\n")
        f_table.write("### OpenMP Execution Time (Seconds)\n\n")
        f_table.write(table_omp.to_markdown() + "\n")
        
    print("Tables successfully saved to 'results/performance_tables.md'")

    plt.tight_layout()
    plt.savefig('results/comprehensive_performance_report.png', dpi=300)
    print("Charts successfully saved to 'results/comprehensive_performance_report.png'")
    plt.show()

except Exception as e:
    print(f"Error during plotting: {e}")