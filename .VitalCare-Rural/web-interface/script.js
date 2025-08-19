/*
 * VitalCare Rural - JavaScript for Real-time Dashboard
 * 
 * This script handles the interactive functionality of the VitalCare Rural
 * web interface including WebSocket communication, patient management,
 * real-time vital signs display, and alert handling.
 * 
 * Author: VitalCare Rural Team
 * Date: August 2025
 * Educational Purpose Only - Not for Medical Use
 */

class VitalCareRural {
    constructor() {
        this.ws = null;
        this.patientRegistered = false;
        this.currentPatient = null;
        this.sessionStartTime = null;
        this.vitalsHistory = {
            heartRate: [],
            bloodPressure: { systolic: [], diastolic: [] },
            spO2: [],
            temperature: []
        };
        this.alerts = [];
        this.maxHistoryPoints = 50;
        
        this.init();
    }

    init() {
        console.log('🏥 VitalCare Rural Dashboard Initializing...');
        
        this.initializeWebSocket();
        this.setupEventListeners();
        this.updateUI();
        this.startStatusUpdates();
        
        console.log('✅ Dashboard Ready!');
    }

    initializeWebSocket() {
        const wsUrl = `ws://${window.location.hostname}:81`;
        console.log(`📡 Connecting to WebSocket: ${wsUrl}`);
        
        try {
            this.ws = new WebSocket(wsUrl);
            
            this.ws.onopen = () => {
                console.log('🔌 WebSocket Connected');
                this.updateConnectionStatus(true);
            };
            
            this.ws.onmessage = (event) => {
                this.handleWebSocketMessage(event);
            };
            
            this.ws.onclose = () => {
                console.log('🔌 WebSocket Disconnected');
                this.updateConnectionStatus(false);
                // Attempt to reconnect after 5 seconds
                setTimeout(() => this.initializeWebSocket(), 5000);
            };
            
            this.ws.onerror = (error) => {
                console.error('❌ WebSocket Error:', error);
                this.updateConnectionStatus(false);
            };
            
        } catch (error) {
            console.error('❌ Failed to initialize WebSocket:', error);
            this.updateConnectionStatus(false);
        }
    }

    handleWebSocketMessage(event) {
        try {
            const data = JSON.parse(event.data);
            
            switch (data.type) {
                case 'init':
                    this.handleInitMessage(data);
                    break;
                case 'vitals':
                    this.handleVitalsUpdate(data);
                    break;
                case 'alert':
                    this.handleAlert(data);
                    break;
                case 'status':
                    this.handleStatusUpdate(data);
                    break;
                default:
                    console.log('📨 Unknown message type:', data.type);
            }
            
            this.updateLastUpdateTime();
            
        } catch (error) {
            console.error('❌ Error parsing WebSocket message:', error);
        }
    }

    handleInitMessage(data) {
        if (data.patientRegistered && data.patient) {
            this.patientRegistered = true;
            this.currentPatient = data.patient;
            this.showPatientInfo();
        }
    }

    handleVitalsUpdate(data) {
        if (!this.patientRegistered) return;
        
        // Update vital signs display
        this.updateVitalSign('heartRate', data.heartRate, 'BPM', this.getHeartRateStatus(data.heartRate));
        this.updateVitalSign('bloodPressure', `${data.systolicBP.toFixed(0)}/${data.diastolicBP.toFixed(0)}`, 'mmHg', this.getBloodPressureStatus(data.systolicBP, data.diastolicBP));
        this.updateVitalSign('spO2', data.spO2.toFixed(0), '%', this.getSpO2Status(data.spO2));
        this.updateVitalSign('temperature', data.temperature.toFixed(1), '°F', this.getTemperatureStatus(data.temperature));
        
        // Add to history for trending
        this.addToHistory(data);
        
        // Update trend charts
        this.updateTrendCharts();
        
        // Check for alerts
        this.checkForAlerts(data);
    }

    updateVitalSign(id, value, unit, status) {
        const valueElement = document.getElementById(id);
        const statusElement = document.getElementById(id === 'bloodPressure' ? 'bpStatus' : `${id.slice(0, -4) || id}Status`);
        
        if (valueElement) {
            valueElement.textContent = value;
        }
        
        if (statusElement) {
            statusElement.textContent = status.text;
            statusElement.className = `status-badge ${status.level}`;
        }
    }

    addToHistory(data) {
        const timestamp = Date.now();
        
        // Add data points and maintain max history
        this.vitalsHistory.heartRate.push({ time: timestamp, value: data.heartRate });
        this.vitalsHistory.bloodPressure.systolic.push({ time: timestamp, value: data.systolicBP });
        this.vitalsHistory.bloodPressure.diastolic.push({ time: timestamp, value: data.diastolicBP });
        this.vitalsHistory.spO2.push({ time: timestamp, value: data.spO2 });
        this.vitalsHistory.temperature.push({ time: timestamp, value: data.temperature });
        
        // Trim history to max points
        Object.keys(this.vitalsHistory).forEach(key => {
            if (Array.isArray(this.vitalsHistory[key])) {
                if (this.vitalsHistory[key].length > this.maxHistoryPoints) {
                    this.vitalsHistory[key] = this.vitalsHistory[key].slice(-this.maxHistoryPoints);
                }
            } else {
                Object.keys(this.vitalsHistory[key]).forEach(subKey => {
                    if (this.vitalsHistory[key][subKey].length > this.maxHistoryPoints) {
                        this.vitalsHistory[key][subKey] = this.vitalsHistory[key][subKey].slice(-this.maxHistoryPoints);
                    }
                });
            }
        });
    }

    updateTrendCharts() {
        // Simple canvas-based trend visualization
        this.drawTrendChart('hrChart', this.vitalsHistory.heartRate, '#e74c3c');
        this.drawTrendChart('bpChart', this.vitalsHistory.bloodPressure.systolic, '#3498db');
        this.drawTrendChart('spo2Chart', this.vitalsHistory.spO2, '#2ecc71');
        this.drawTrendChart('tempChart', this.vitalsHistory.temperature, '#f39c12');
    }

    drawTrendChart(canvasId, data, color) {
        const canvas = document.getElementById(canvasId);
        if (!canvas || !data.length) return;
        
        const ctx = canvas.getContext('2d');
        const width = canvas.width;
        const height = canvas.height;
        
        // Clear canvas
        ctx.clearRect(0, 0, width, height);
        
        if (data.length < 2) return;
        
        // Find min/max for scaling
        const values = data.map(point => point.value);
        const min = Math.min(...values);
        const max = Math.max(...values);
        const range = max - min || 1;
        
        // Draw line
        ctx.strokeStyle = color;
        ctx.lineWidth = 2;
        ctx.beginPath();
        
        data.forEach((point, index) => {
            const x = (index / (data.length - 1)) * width;
            const y = height - ((point.value - min) / range) * height;
            
            if (index === 0) {
                ctx.moveTo(x, y);
            } else {
                ctx.lineTo(x, y);
            }
        });
        
        ctx.stroke();
    }

    checkForAlerts(data) {
        const alerts = [];
        
        // Heart rate alerts
        if (data.heartRate < 60) {
            alerts.push({ type: 'warning', message: 'Heart rate below normal (Bradycardia)', vital: 'Heart Rate', value: data.heartRate });
        } else if (data.heartRate > 100) {
            alerts.push({ type: 'warning', message: 'Heart rate above normal (Tachycardia)', vital: 'Heart Rate', value: data.heartRate });
        }
        
        // Blood pressure alerts
        if (data.systolicBP > 140 || data.diastolicBP > 90) {
            alerts.push({ type: 'critical', message: 'High blood pressure detected', vital: 'Blood Pressure', value: `${data.systolicBP}/${data.diastolicBP}` });
        } else if (data.systolicBP < 90 || data.diastolicBP < 60) {
            alerts.push({ type: 'warning', message: 'Low blood pressure detected', vital: 'Blood Pressure', value: `${data.systolicBP}/${data.diastolicBP}` });
        }
        
        // SpO2 alerts
        if (data.spO2 < 95) {
            alerts.push({ type: 'critical', message: 'Low oxygen saturation', vital: 'SpO2', value: data.spO2 });
        }
        
        // Temperature alerts
        if (data.temperature > 100.4) {
            alerts.push({ type: 'warning', message: 'Fever detected', vital: 'Temperature', value: data.temperature });
        } else if (data.temperature < 97.0) {
            alerts.push({ type: 'warning', message: 'Low body temperature', vital: 'Temperature', value: data.temperature });
        }
        
        // Add new alerts
        alerts.forEach(alert => this.addAlert(alert));
    }

    addAlert(alert) {
        const alertWithTime = {
            ...alert,
            timestamp: new Date(),
            id: Date.now() + Math.random()
        };
        
        this.alerts.unshift(alertWithTime);
        this.updateAlertPanel();
        
        // Show alert panel if hidden
        const alertPanel = document.getElementById('alertPanel');
        if (alertPanel) {
            alertPanel.style.display = 'block';
        }
    }

    updateAlertPanel() {
        const alertsList = document.getElementById('alertsList');
        if (!alertsList) return;
        
        if (this.alerts.length === 0) {
            alertsList.innerHTML = '<p class="text-center" style="color: #6c757d; padding: 2rem;">No alerts</p>';
            return;
        }
        
        alertsList.innerHTML = this.alerts.slice(0, 10).map(alert => `
            <div class="alert-item ${alert.type}">
                <div>
                    <strong>${alert.vital}:</strong> ${alert.message}
                    <br>
                    <small class="alert-time">Value: ${alert.value} | ${alert.timestamp.toLocaleTimeString()}</small>
                </div>
            </div>
        `).join('');
    }

    setupEventListeners() {
        // Patient registration form
        const patientForm = document.getElementById('patientForm');
        if (patientForm) {
            patientForm.addEventListener('submit', (e) => this.handlePatientRegistration(e));
        }
        
        // New patient button
        const newPatientBtn = document.getElementById('newPatientBtn');
        if (newPatientBtn) {
            newPatientBtn.addEventListener('click', () => this.startNewPatient());
        }
        
        // Action buttons
        const saveReadingBtn = document.getElementById('saveReadingBtn');
        if (saveReadingBtn) {
            saveReadingBtn.addEventListener('click', () => this.saveReading());
        }
        
        const emergencyBtn = document.getElementById('emergencyBtn');
        if (emergencyBtn) {
            emergencyBtn.addEventListener('click', () => this.triggerEmergencyAlert());
        }
        
        const printReportBtn = document.getElementById('printReportBtn');
        if (printReportBtn) {
            printReportBtn.addEventListener('click', () => this.generateReport());
        }
        
        const clearAlertsBtn = document.getElementById('clearAlertsBtn');
        if (clearAlertsBtn) {
            clearAlertsBtn.addEventListener('click', () => this.clearAlerts());
        }
    }

    async handlePatientRegistration(e) {
        e.preventDefault();
        
        const formData = {
            name: document.getElementById('patientName').value.trim(),
            age: parseInt(document.getElementById('patientAge').value),
            gender: document.getElementById('patientGender').value,
            contact: document.getElementById('patientContact').value.trim(),
            emergencyContact: document.getElementById('emergencyContact').value.trim(),
            medicalConditions: document.getElementById('medicalConditions').value.trim()
        };
        
        // Validate required fields
        if (!formData.name || !formData.age || !formData.gender) {
            alert('Please fill in all required fields (Name, Age, Gender)');
            return;
        }
        
        try {
            const response = await fetch('/api/register-patient', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(formData)
            });
            
            const result = await response.json();
            
            if (result.success) {
                this.patientRegistered = true;
                this.currentPatient = formData;
                this.sessionStartTime = new Date();
                this.showPatientInfo();
                console.log('✅ Patient registered successfully');
            } else {
                alert('Failed to register patient: ' + result.message);
            }
            
        } catch (error) {
            console.error('❌ Error registering patient:', error);
            alert('Error registering patient. Please try again.');
        }
    }

    showPatientInfo() {
        // Hide registration form
        const patientRegistration = document.getElementById('patientRegistration');
        if (patientRegistration) {
            patientRegistration.style.display = 'none';
        }
        
        // Show patient info
        const patientInfo = document.getElementById('patientInfo');
        if (patientInfo) {
            patientInfo.style.display = 'block';
        }
        
        // Show vitals section
        const vitalsSection = document.getElementById('vitalsSection');
        if (vitalsSection) {
            vitalsSection.style.display = 'block';
        }
        
        // Show action buttons
        const actionButtons = document.getElementById('actionButtons');
        if (actionButtons) {
            actionButtons.style.display = 'block';
        }
        
        // Update patient info display
        document.getElementById('currentPatientName').textContent = this.currentPatient.name;
        document.getElementById('currentPatientAge').textContent = this.currentPatient.age;
        document.getElementById('currentPatientGender').textContent = this.currentPatient.gender;
        
        // Start session timer
        this.startSessionTimer();
    }

    startNewPatient() {
        if (confirm('Are you sure you want to start monitoring a new patient? Current session data will be saved.')) {
            this.patientRegistered = false;
            this.currentPatient = null;
            this.sessionStartTime = null;
            this.vitalsHistory = {
                heartRate: [],
                bloodPressure: { systolic: [], diastolic: [] },
                spO2: [],
                temperature: []
            };
            this.alerts = [];
            
            this.updateUI();
        }
    }

    startSessionTimer() {
        setInterval(() => {
            if (this.sessionStartTime) {
                const elapsed = new Date() - this.sessionStartTime;
                const hours = Math.floor(elapsed / (1000 * 60 * 60));
                const minutes = Math.floor((elapsed % (1000 * 60 * 60)) / (1000 * 60));
                const seconds = Math.floor((elapsed % (1000 * 60)) / 1000);
                
                const sessionTime = document.getElementById('sessionTime');
                if (sessionTime) {
                    sessionTime.textContent = `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
                }
            }
        }, 1000);
    }

    updateUI() {
        if (this.patientRegistered) {
            this.showPatientInfo();
        } else {
            // Show registration form
            const patientRegistration = document.getElementById('patientRegistration');
            if (patientRegistration) {
                patientRegistration.style.display = 'block';
            }
            
            // Hide other sections
            ['patientInfo', 'vitalsSection', 'actionButtons', 'alertPanel'].forEach(id => {
                const element = document.getElementById(id);
                if (element) {
                    element.style.display = 'none';
                }
            });
        }
    }

    updateConnectionStatus(connected) {
        const statusElement = document.getElementById('connectionStatus');
        if (statusElement) {
            if (connected) {
                statusElement.textContent = '🟢 Connected';
                statusElement.style.color = '#28a745';
            } else {
                statusElement.textContent = '🔴 Disconnected';
                statusElement.style.color = '#dc3545';
            }
        }
    }

    updateLastUpdateTime() {
        const timestampElement = document.getElementById('lastUpdate');
        if (timestampElement) {
            timestampElement.textContent = `Last Update: ${new Date().toLocaleTimeString()}`;
        }
    }

    startStatusUpdates() {
        // Update system status periodically
        setInterval(() => {
            // Update WiFi status based on WebSocket connection
            const wifiStatus = document.getElementById('wifiStatus');
            if (wifiStatus) {
                wifiStatus.textContent = this.ws && this.ws.readyState === WebSocket.OPEN ? 'Connected' : 'Disconnected';
                wifiStatus.style.color = this.ws && this.ws.readyState === WebSocket.OPEN ? '#28a745' : '#dc3545';
            }
        }, 5000);
    }

    // Utility functions for vital sign status
    getHeartRateStatus(hr) {
        if (hr < 60) return { text: 'Bradycardia', level: 'warning' };
        if (hr > 100) return { text: 'Tachycardia', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    getBloodPressureStatus(systolic, diastolic) {
        if (systolic > 140 || diastolic > 90) return { text: 'High', level: 'critical' };
        if (systolic < 90 || diastolic < 60) return { text: 'Low', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    getSpO2Status(spo2) {
        if (spo2 < 95) return { text: 'Low', level: 'critical' };
        if (spo2 < 97) return { text: 'Below Normal', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    getTemperatureStatus(temp) {
        if (temp > 100.4) return { text: 'Fever', level: 'warning' };
        if (temp < 97.0) return { text: 'Low', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    // Action button handlers
    saveReading() {
        if (!this.patientRegistered) {
            alert('No patient registered');
            return;
        }
        
        // TODO: Implement save reading functionality
        alert('Reading saved successfully!');
        console.log('💾 Reading saved for patient:', this.currentPatient.name);
    }

    triggerEmergencyAlert() {
        if (!this.patientRegistered) {
            alert('No patient registered');
            return;
        }
        
        if (confirm('Are you sure you want to trigger an emergency alert?')) {
            // TODO: Implement emergency alert functionality
            alert('Emergency alert sent!');
            console.log('🚨 Emergency alert triggered for patient:', this.currentPatient.name);
        }
    }

    generateReport() {
        if (!this.patientRegistered) {
            alert('No patient registered');
            return;
        }
        
        // TODO: Implement report generation
        alert('Report generated successfully!');
        console.log('📄 Report generated for patient:', this.currentPatient.name);
    }

    clearAlerts() {
        this.alerts = [];
        this.updateAlertPanel();
        
        const alertPanel = document.getElementById('alertPanel');
        if (alertPanel && this.alerts.length === 0) {
            alertPanel.style.display = 'none';
        }
    }
}

// Initialize the application when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    window.vitalCareRural = new VitalCareRural();
});

// Handle page visibility changes
document.addEventListener('visibilitychange', () => {
    if (window.vitalCareRural) {
        if (document.hidden) {
            console.log('📱 Page hidden - reducing update frequency');
        } else {
            console.log('📱 Page visible - resuming normal updates');
        }
    }
});

// Handle network status changes
window.addEventListener('online', () => {
    console.log('🌐 Network connection restored');
    if (window.vitalCareRural) {
        window.vitalCareRural.initializeWebSocket();
    }
});

window.addEventListener('offline', () => {
    console.log('🌐 Network connection lost');
});
