# TODO:
# After adding AMAT in csv, provide option to plot any of the 3 (hit, miss, AMAT) against each other, instead of the hit v misscount right now

import streamlit as st
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import sys

def is_power_of_two(n):
    return n>0 and (n & (n-1)) == 0

def plot_cache_data(csv_file, output_png):
    data = pd.read_csv(csv_file)

    plt.figure(figsize=(15, 9))
    plt.plot(data['Step'], data['HitCount'], label='Hit Count')
    plt.plot(data['Step'], data['MissCount'], label='Miss Count')
    plt.xlabel('Step')
    plt.ylabel('Count')
    plt.title('Cache Hit and Miss Counts Over Time')
    plt.legend()
    plt.grid(True)

    st.pyplot(plt)
    plt.savefig(output_png)

    print(f"Plot saved as {output_png}")

def run_cache(max_size, k, block_size):
    subprocess.call(["g++", "pyexec.cpp"])
    subprocess.call(["./a.out", str(max_size), str(k), str(block_size)])
    plot_cache_data("pyexec.csv", "pyexec") 
    st.markdown("## maxSize")
    st.write(f"MAX SIZE: {max_size} bytes")
    st.write(f"K: {k}")
    st.write(f"BLOCK SIZE: {block_size} bytes")

def main():
    st.markdown("""<hr style="border:3px solid rgb(255,255,255) ">""", unsafe_allow_html=True)
    st.markdown("""# K-WAY SET-ASSOCIATIVE CACHE SIMULATION""")
    st.markdown("""<hr style="border:3px solid rgb(255,255,255) ">""", unsafe_allow_html=True)

    MAXSIZE_input = st.number_input("Enter MAX SIZE of cache (bytes)", value=128)
    K_input = st.number_input("Enter the value of K", value=4)
    BLOCKSIZE_input = st.number_input("Enter the block size (bytes)", value=4)
    
    if(not (is_power_of_two(K_input))):
        st.error(f"{K_input} is not a power of 2")
    
    if st.button(label="SIMULATE"):
        run_cache(MAXSIZE_input, K_input, BLOCKSIZE_input)

if __name__ == "__main__":
    main()
