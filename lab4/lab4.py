import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button, CheckButtons
from scipy.signal import butter, filtfilt

t_min, t_max, num_points = 0, 10, 1000
t = np.linspace(t_min, t_max, num_points)
fs = num_points / (t_max - t_min)

base_noise = np.random.normal(0, 1, num_points)

init_amp = 1.0
init_freq = 0.5
init_phase = 0.0
init_noise_mean = 0.0
init_noise_cov = 0.1
init_cutoff = 2.0
init_show_noise = True

def harmonic_with_noise(t, amplitude, frequency, phase, noise_mean, noise_covariance, show_noise):
    """Генерує чисту гармоніку та гармоніку з шумом."""
    omega = 2 * np.pi * frequency
    y_clean = amplitude * np.sin(omega * t + phase)
    
    if show_noise:
        std_dev = np.sqrt(max(0, noise_covariance)) 
        current_noise = noise_mean + std_dev * base_noise
        y_noisy = y_clean + current_noise
    else:
        y_noisy = y_clean
        
    return y_clean, y_noisy

def apply_filter(data, cutoff, fs):
    """Фільтрує сигнал за допомогою ФНЧ Баттерворта."""
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    if normal_cutoff >= 1.0 or normal_cutoff <= 0:
        return data
    
    b, a = butter(4, normal_cutoff, btype='low', analog=False)
    filtered_data = filtfilt(b, a, data)
    return filtered_data

fig, ax = plt.subplots(figsize=(10, 7))
plt.subplots_adjust(bottom=0.45)

ax.set_title("Інтерактивна візуалізація гармоніки та фільтрації")
ax.set_xlabel("Час (t)")
ax.set_ylabel("Амплітуда y(t)")
ax.grid(True, linestyle='--', alpha=0.7)

y_clean, y_noisy = harmonic_with_noise(t, init_amp, init_freq, init_phase, init_noise_mean, init_noise_cov, init_show_noise)
y_filtered = apply_filter(y_noisy, init_cutoff, fs)

line_noisy, = ax.plot(t, y_noisy, color='orange', label='Зашумлений сигнал', alpha=0.7)
line_filtered, = ax.plot(t, y_filtered, color='blue', label='Відфільтрований сигнал', linewidth=2, linestyle='--')
line_clean, = ax.plot(t, y_clean, color='red', label='Чиста гармоніка', linewidth=2)
ax.legend(loc='upper right')

axcolor = 'lightgoldenrodyellow'
ax_amp    = plt.axes([0.15, 0.35, 0.65, 0.03], facecolor=axcolor)
ax_freq   = plt.axes([0.15, 0.30, 0.65, 0.03], facecolor=axcolor)
ax_phase  = plt.axes([0.15, 0.25, 0.65, 0.03], facecolor=axcolor)
ax_nmean  = plt.axes([0.15, 0.20, 0.65, 0.03], facecolor=axcolor)
ax_ncov   = plt.axes([0.15, 0.15, 0.65, 0.03], facecolor=axcolor)
ax_cutoff = plt.axes([0.15, 0.10, 0.65, 0.03], facecolor='lightblue')

slider_amp    = Slider(ax_amp, 'Амплітуда', 0.1, 5.0, valinit=init_amp)
slider_freq   = Slider(ax_freq, 'Частота', 0.1, 5.0, valinit=init_freq)
slider_phase  = Slider(ax_phase, 'Фаза', 0.0, 2 * np.pi, valinit=init_phase)
slider_nmean  = Slider(ax_nmean, 'Шум (Середнє)', -2.0, 2.0, valinit=init_noise_mean)
slider_ncov   = Slider(ax_ncov, 'Шум (Дисперсія)', 0.0, 2.0, valinit=init_noise_cov)
slider_cutoff = Slider(ax_cutoff, 'Зріз фільтра', 0.1, 20.0, valinit=init_cutoff)

ax_check = plt.axes([0.85, 0.30, 0.12, 0.1])
check = CheckButtons(ax_check, ['Шум ON/OFF'], [init_show_noise])

ax_reset = plt.axes([0.85, 0.15, 0.1, 0.04])
button_reset = Button(ax_reset, 'Reset', hovercolor='0.975')

def update(val=None):
    show_noise = check.get_status()[0]
    
    y_cln, y_nsy = harmonic_with_noise(
        t, slider_amp.val, slider_freq.val, slider_phase.val, 
        slider_nmean.val, slider_ncov.val, show_noise
    )
    y_flt = apply_filter(y_nsy, slider_cutoff.val, fs)
    
    line_clean.set_ydata(y_cln)
    line_noisy.set_ydata(y_nsy)
    line_filtered.set_ydata(y_flt)
    
    ax.relim()
    ax.autoscale_view(scalex=False, scaley=True)
    fig.canvas.draw_idle()

def reset(event):
    slider_amp.reset()
    slider_freq.reset()
    slider_phase.reset()
    slider_nmean.reset()
    slider_ncov.reset()
    slider_cutoff.reset()
    if not check.get_status()[0]:
        check.set_active(0)
    update()

slider_amp.on_changed(update)
slider_freq.on_changed(update)
slider_phase.on_changed(update)
slider_nmean.on_changed(update)
slider_ncov.on_changed(update)
slider_cutoff.on_changed(update)
check.on_clicked(update)
button_reset.on_clicked(reset)

plt.show()