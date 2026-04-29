import streamlit as st
import pandas as pd
import plotly.express as px
import os
import glob

st.set_page_config(page_title="ЗПАД Лабораторна 5", layout="wide")

@st.cache_data
def load_and_merge_data(folder_path='vhi_dataset'):
    """Зчитує всі 25 CSV файлів та об'єднує їх в один DataFrame, жорстко ігноруючи сміття."""
    all_files = glob.glob(os.path.join(folder_path, "*.csv"))
    
    if not all_files:
        return pd.DataFrame()

    province_names = {
        1: 'Вінницька', 2: 'Волинська', 3: 'Дніпропетровська', 4: 'Донецька', 
        5: 'Житомирська', 6: 'Закарпатська', 7: 'Запорізька', 8: 'Івано-Франківська', 
        9: 'Київська', 10: 'Кіровоградська', 11: 'Луганська', 12: 'Львівська', 
        13: 'Миколаївська', 14: 'Одеська', 15: 'Полтавська', 16: 'Рівненська', 
        17: 'Сумська', 18: 'Тернопільська', 19: 'Харківська', 20: 'Херсонська', 
        21: 'Хмельницька', 22: 'Черкаська', 23: 'Чернівецька', 24: 'Чернігівська', 25: 'Крим'
    }

    df_list = []
    for file in all_files:
        df = pd.read_csv(
            file, 
            header=1, 
            names=['Year', 'Week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI', 'empty'],
            skipinitialspace=True,
            engine='python'
        )
        df = df.drop(columns=['empty'], errors='ignore')
        
        df['Year'] = df['Year'].astype(str).str.replace('<tt><pre>', '', regex=False)
        df['Year'] = df['Year'].str.replace('</pre></tt>', '', regex=False)
        df['Year'] = df['Year'].str.strip()
        
        df = df[df['Year'].str.isnumeric()]
        
        filename = os.path.basename(file)
        try:
            prov_id = int(filename.split('_')[1])
            df['Province'] = province_names.get(prov_id, f'Область {prov_id}')
        except (IndexError, ValueError):
            df['Province'] = 'Невідома область'
            
        df_list.append(df)
        
    full_df = pd.concat(df_list, ignore_index=True)
    
    full_df = full_df.dropna(subset=['Year', 'Week', 'VHI', 'VCI', 'TCI'])
    full_df = full_df[full_df['VHI'] != -1]
    
    full_df['Year'] = full_df['Year'].astype(int)
    full_df['Week'] = full_df['Week'].astype(int)
    
    return full_df

df = load_and_merge_data()

if df.empty:
    st.error("Помилка: Не вдалося знайти файли в папці vhi_dataset. Перевірте шлях!")
    st.stop()

def reset_all():
    st.session_state['index_sel'] = 'VHI'
    st.session_state['prov_sel'] = 'Київська' if 'Київська' in df['Province'].unique() else df['Province'].unique()[0]
    st.session_state['week_range'] = (1, 52)
    st.session_state['year_range'] = (int(df['Year'].min()), int(df['Year'].max()))
    st.session_state['sort_asc'] = False
    st.session_state['sort_desc'] = False

if 'index_sel' not in st.session_state:
    reset_all()

col_controls, col_results = st.columns([1, 3])

with col_controls:
    st.header(" Фільтри")
    
    target_index = st.selectbox("Показник:", ['VCI', 'TCI', 'VHI'], key='index_sel')
    target_province = st.selectbox("Область:", sorted(df['Province'].unique()), key='prov_sel')
    
    weeks = st.slider("Тижні:", 1, 52, key='week_range')
    years = st.slider("Роки:", int(df['Year'].min()), int(df['Year'].max()), key='year_range')
    
    st.divider()
    st.subheader("Сортування таблиці")
    s_asc = st.checkbox("За зростанням", key='sort_asc')
    s_desc = st.checkbox("За спаданням", key='sort_desc')
    
    st.button("Скинути все", on_click=reset_all, use_container_width=True)

filtered_df = df[
    (df['Province'] == target_province) &
    (df['Year'].between(years[0], years[1])) &
    (df['Week'].between(weeks[0], weeks[1]))
].copy()

if s_asc and s_desc:
    st.warning(" Обрано обидва типи сортування. Сортування вимкнено.")
elif s_asc:
    filtered_df = filtered_df.sort_values(by=target_index, ascending=True)
elif s_desc:
    filtered_df = filtered_df.sort_values(by=target_index, ascending=False)

comparison_df = df[
    (df['Year'].between(years[0], years[1])) &
    (df['Week'].between(weeks[0], weeks[1]))
].groupby('Province')[target_index].mean().reset_index()

with col_results:
    st.header(f"Аналіз даних для {target_province}")
    
    tab_table, tab_chart, tab_compare = st.tabs(["Таблиця", "Динаміка", "Порівняння"])
    
    with tab_table:
        st.subheader("Відфільтровані дані")
        st.dataframe(filtered_df[['Year', 'Week', target_index]], use_container_width=True)
        
    with tab_chart:
        st.subheader(f"Часовий ряд {target_index}")
        filtered_df['Period'] = filtered_df['Year'].astype(str) + "-W" + filtered_df['Week'].astype(str)
        plot_df = filtered_df.sort_values(['Year', 'Week'])
        
        fig1 = px.line(plot_df, x='Period', y=target_index, 
                       title=f"Зміна {target_index} у часі",
                       markers=True)
        st.plotly_chart(fig1, use_container_width=True)
        
    with tab_compare:
        st.subheader(f"Середній {target_index} по областях")
        comparison_df['Status'] = comparison_df['Province'].apply(lambda x: 'Обрана' if x == target_province else 'Інші')
        
        fig2 = px.bar(comparison_df.sort_values(target_index), 
                      x='Province', y=target_index, color='Status',
                      color_discrete_map={'Обрана': '#FF4B4B', 'Інші': '#636EFA'},
                      title=f"Порівняння {target_index} за вказаний період")
        st.plotly_chart(fig2, use_container_width=True)