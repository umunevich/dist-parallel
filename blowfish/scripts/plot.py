import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


sns.set_theme(style="whitegrid")

try:
    df_mpi = pd.read_csv('results/mpi.csv')
    df_omp = pd.read_csv('results/omp.csv')

    if df_mpi.empty or df_omp.empty:
        raise ValueError("Один із файлів CSV порожній. Запустіть bench-скрипти!")

    t1_mpi = df_mpi[df_mpi['processes'] == 1]['time'].values[0]
    df_mpi['speedup'] = t1_mpi / df_mpi['time']

    t1_omp = df_omp[df_omp['threads'] == 1]['time'].values[0]
    df_omp['speedup'] = t1_omp / df_omp['time']


    fig, axes = plt.subplots(1, 2, figsize=(16, 6))

    # 1: Порівняння прискорення
    axes[0].plot(df_mpi['processes'], df_mpi['speedup'], 'o-', label='MPI Speedup', color='tab:red')
    axes[0].plot(df_omp['threads'], df_omp['speedup'], 's-', label='OpenMP Speedup', color='tab:green')

    max_workers = max(df_mpi['processes'].max(), df_omp['threads'].max())
    axes[0].plot([1, max_workers], [1, max_workers], '--', color='gray', label='Ideal Speedup')
    
    axes[0].set_title('Speedup Comparison', fontsize=14)
    axes[0].set_xlabel('Number of Workers')
    axes[0].set_ylabel('Speedup Factor')
    axes[0].legend()
    axes[0].grid(True)

    # 2: Пряме порівняння часу
    axes[1].plot(df_mpi['processes'], df_mpi['time'], 'o-', label='MPI Time', color='tab:red')
    axes[1].plot(df_omp['threads'], df_omp['time'], 's-', label='OpenMP Time', color='tab:green')
    axes[1].set_title('MPI vs OpenMP Execution Time', fontsize=14)
    axes[1].set_xlabel('Number of Workers')
    axes[1].set_ylabel('Time (seconds)')
    axes[1].legend()
    axes[1].grid(True)

    plt.tight_layout()
    plt.savefig('results/performance_detailed.png', dpi=300)
    plt.show()

except Exception as e:
    print(f"Помилка при побудові графіків: {e}")