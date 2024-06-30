# TODO:
# After adding AMAT in csv, provide option to plot any of the 3 (hit, miss, AMAT) against each other, instead of the hit v misscount right now

import streamlit as st
import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import sys

def is_power_of_two(n):
    return n>0 and (n & (n-1)) == 0

def plot_cache_data(csv_file, output_png, field):
    data = pd.read_csv(csv_file)

    # 'MissCount', 'HitCount', 'AMAT', 'MissRatio'
    plt.figure(figsize=(15, 9))
    plt.plot(data['Step'], data[field], label=field)
    plt.xlabel('Step')
    plt.ylabel('Count')
    plt.title(f'{field} Over Time')
    plt.legend()
    plt.grid(True)

    st.pyplot(plt)

def run_cache(max_size, k, block_size, step_size):
    subprocess.call(["g++", "pyexec.cpp"])
    subprocess.call(["./a.out", str(max_size), str(k), str(block_size), str(step_size)])

def main():
    st.markdown("""<hr style="border:3px solid rgb(255,255,255) ">""", unsafe_allow_html=True)
    st.markdown("""# K-WAY SET-ASSOCIATIVE CACHE SIMULATION""")
    st.markdown("""<hr style="border:3px solid rgb(255,255,255) ">""", unsafe_allow_html=True)

    MAXSIZE_input = st.number_input("Enter MAX SIZE of cache (bytes)", value=128)
    K_input = st.number_input("Enter the value of K", value=4)
    BLOCKSIZE_input = st.number_input("Enter the block size (bytes)", value=16)
    STEP_input = st.number_input("Enter the step size ", value=10)
    

    options = st.multiselect("Select fields", ['MissCount', 'HitCount', 'AMAT', 'MissRatio'], ['MissRatio', 'AMAT'])

    st.write("You selected:", options)
    

    if(not (is_power_of_two(K_input))):
        st.error(f"{K_input} is not a power of 2")
    
    if st.button(label="SIMULATE"):
        run_cache(MAXSIZE_input, K_input, BLOCKSIZE_input, STEP_input)
        st.markdown("""<hr style="border:3px solid rgb(255,255,255) ">""", unsafe_allow_html=True)
        for field in options:
            st.markdown(f"""### {field}:""")
            plot_cache_data("pyexec.csv", "pyexec", field) 
            st.markdown("""<hr style="border:3px solid rgb(255,255,255) ">""", unsafe_allow_html=True)

if __name__ == "__main__":
    main()
