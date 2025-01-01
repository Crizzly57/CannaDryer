// Stores the current general settings locally
let generalSettings = {
    runtimeOfFans: 0,
    interval: 0
};

const fanNameMapping = {
    'fan-front': 'Front',
    'fan-back': 'Back'
};

// Helper function to update the fan data display (speed, power, and sliders)
function updateFanData(fanName, fanData, updateSlider = false) {
    // Update fan speed and power
    document.getElementById(`${fanName}-speed`).textContent = `${fanData.speed} RPM`;
    document.getElementById(`${fanName}-power`).textContent = `${fanData.power}%`;

    // Update the slider and display power percentage if required
    if (updateSlider) {
        const fanSlider = document.getElementById(`${fanName}-power-slider`);
        fanSlider.value = fanData.power;  // Set the slider to the current value
        document.getElementById(`${fanName}-power-display`).textContent = `${fanData.power}%`;
    }
}

// Function to fetch and display fan data (Fan 1 and Fan 2)
async function fetchFanData(updateSliders = false) {
    try {
        // Fetch data for Fans
        const response = await fetch('/fan');

        // Handle the response
        const fanData = await response.json();
        
        // Update the fan data displays and sliders
        updateFanData('fan-front', fanData[0], updateSliders);
        updateFanData('fan-back', fanData[1], updateSliders);

    } catch (error) {
        console.error('Error fetching fan data:', error);
    }
}

// Function to fetch general settings on page load
async function fetchGeneralConfig() {
    try {
        const response = await fetch('/fanManager');
        const settings = await response.json();

        // Update local general settings and input fields
        generalSettings.runtimeOfFans = settings.runtimeOfFans;
        generalSettings.interval = settings.interval;
        document.getElementById('runtime-of-fans').value = settings.runtimeOfFans;
        document.getElementById('interval').value = settings.interval;

    } catch (error) {
        console.error('Error fetching general settings:', error);
    }
}

// Function to update general configuration on user input
async function setGeneralConfig() {
    const runtimeOfFans = document.getElementById('runtime-of-fans').value;
    const interval = document.getElementById('interval').value;

    // Update the local settings object
    generalSettings.runtimeOfFans = parseInt(runtimeOfFans);
    generalSettings.interval = parseInt(interval);

    const body = JSON.stringify({
        runtimeOfFans: generalSettings.runtimeOfFans,
        interval: generalSettings.interval
    });

    try {
        const response = await fetch('/fanManager', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: body
        });

        if (response.ok) {
            console.log('General config updated');
        } else {
            console.error('Error updating general config');
        }
    } catch (error) {
        console.error('Error sending general config update:', error);
    }
}

// Function to update fan power (Front or Back)
async function setFanPower(fanName, power) {
    const endpoint = `/fan?name=${fanNameMapping[fanName]}`;
    const body = JSON.stringify({ power: power });

    try {
        const response = await fetch(endpoint, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: body,
        });

        if (!response.ok) {
            throw new Error(`Error setting power for Fan ${fanName}`);
        }

        // Update the power display
        document.getElementById(`${fanName}-power`).textContent = `${power}%`;

        console.log(`Fan ${fanName} power set to ${power}`);
    } catch (error) {
        console.error(`Error setting power for Fan ${fanName}:`, error);
    }
}

// Function to set up event listeners for the fan power sliders
function setupSliders() {
    const sliders = document.querySelectorAll('.fan-power-slider');

    sliders.forEach(slider => {
        slider.addEventListener('input', (event) => {
            const fanName = event.target.dataset.fanName;
            document.getElementById(`${fanName}-power-display`).textContent = `${event.target.value} %`;
        });

        slider.addEventListener('change', (event) => {
            const fanName = event.target.dataset.fanName;
            setFanPower(fanName, parseInt(event.target.value, 10));
        });
    });
}

// Function to start periodically fetching fan data every 2 seconds
function startFetchingFanData() {
    setInterval(() => fetchFanData(false), 2000); // Fetch data every 2 seconds
}

// Initialize the page
async function initialize() {
    await fetchFanData(true);       // Initial fan data fetch and slider update
    await fetchGeneralConfig();     // Fetch and set general settings
    setupSliders();                 // Set up slider event listeners
    startFetchingFanData();         // Start fetching fan data periodically
}

// Run initialization when the page is loaded
window.onload = initialize;