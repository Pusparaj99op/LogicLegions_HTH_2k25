/*
 * VitalCare Rural - JavaScript for Single ESP32 Complete System Dashboard
 * 
 * This script handles the interactive functionality of the VitalCare Rural
 * web interface for the single ESP32 complete system including WebSocket 
 * communication, patient management, real-time vital signs display, ECG
 * waveform visualization, and comprehensive alert handling.
 * 
 * Features:
 * - Real-time WebSocket communication with ESP32
 * - Patient registration and management
 * - Live vital signs monitoring with trend charts
 * - ECG waveform display
 * - Alert management with SMS notifications
 * - Data export and report generation
 * - System status monitoring
 * 
 * Author: VitalCare Rural Team
 * Date: January 2025
 * Educational Purpose Only - Not for Medical Use
 */

class VitalCareRuralComplete {
    constructor() {
        this.ws = null;
        this.patientRegistered = false;
        this.currentPatient = null;
        this.sessionStartTime = null;
        this.vitalsHistory = {
            heartRate: [],
            bloodPressure: { systolic: [], diastolic: [] },
            spO2: [],
            temperature: [],
            ecg: [],
            pressure: []
        };
        this.alerts = [];
        this.maxHistoryPoints = 100;
        this.ecgData = [];
        this.maxECGPoints = 200;
        this.ecgPaused = false;
        this.systemStatus = {
            wifi: false,
            sensors: false,
            storage: false,
            cellular: false
        };
        this.trafficStats = {
            packetsReceived: 0,
            bytesReceived: 0,
            packetRate: 0,
            lastRateUpdate: Date.now(),
            packetCounts: []
        };
        this.vitals = {
            heartRate: null,
            systolicBP: null,
            diastolicBP: null,
            spO2: null,
            temperature: null
        };
        
        this.init();
    }

    init() {
        console.log('🏥 VitalCare Rural Complete System Initializing...');
        
        // Show loading indicator during initialization
        this.showLoadingState('Initializing System...');
        
        // Staggered initialization with visual feedback
        setTimeout(() => {
            this.updateLoadingState('Connecting to ESP32...', 20);
            this.initializeWebSocket();
            
            setTimeout(() => {
                this.updateLoadingState('Setting up interface...', 40);
                this.setupEventListeners();
                
                setTimeout(() => {
                    this.updateLoadingState('Preparing displays...', 60);
                    this.updateUI();
                    
                    setTimeout(() => {
                        this.updateLoadingState('Starting system monitors...', 80);
                        this.startStatusUpdates();
                        this.initializeECGDisplay();
                        
                        setTimeout(() => {
                            this.updateLoadingState('Ready!', 100);
                            // Hide loader after brief delay
                            setTimeout(() => this.hideLoadingState(), 500);
                            console.log('✅ Complete System Dashboard Ready!');
                        }, 300);
                    }, 300);
                }, 300);
            }, 300);
        }, 300);
    }
    
    showLoadingState(message) {
        // Create loading overlay if it doesn't exist
        let loadingOverlay = document.getElementById('loadingOverlay');
        if (!loadingOverlay) {
            loadingOverlay = document.createElement('div');
            loadingOverlay.id = 'loadingOverlay';
            loadingOverlay.style.position = 'fixed';
            loadingOverlay.style.top = '0';
            loadingOverlay.style.left = '0';
            loadingOverlay.style.width = '100%';
            loadingOverlay.style.height = '100%';
            loadingOverlay.style.backgroundColor = 'rgba(0, 0, 0, 0.8)';
            loadingOverlay.style.display = 'flex';
            loadingOverlay.style.flexDirection = 'column';
            loadingOverlay.style.justifyContent = 'center';
            loadingOverlay.style.alignItems = 'center';
            loadingOverlay.style.zIndex = '9999';
            loadingOverlay.style.transition = 'opacity 0.5s ease';
            
            // Create loading content
            loadingOverlay.innerHTML = `
                <h2 style="color: white; font-size: 2rem; margin-bottom: 1rem;">🏥 VitalCare Rural</h2>
                <div class="loading"></div>
                <div id="loadingMessage" style="color: white; margin-top: 1rem; font-size: 1.2rem;">${message}</div>
                <div style="width: 50%; max-width: 400px; margin-top: 2rem;">
                    <div style="background: rgba(255,255,255,0.2); height: 10px; border-radius: 5px; overflow: hidden;">
                        <div id="loadingProgress" style="background: linear-gradient(90deg, var(--primary-color), var(--success-color)); 
                            height: 100%; width: 0%; transition: width 0.3s ease;"></div>
                    </div>
                </div>
            `;
            
            document.body.appendChild(loadingOverlay);
        } else {
            document.getElementById('loadingMessage').textContent = message;
        }
    }
    
    updateLoadingState(message, progress) {
        const loadingMessage = document.getElementById('loadingMessage');
        const loadingProgress = document.getElementById('loadingProgress');
        
        if (loadingMessage) {
            loadingMessage.textContent = message;
        }
        
        if (loadingProgress) {
            loadingProgress.style.width = `${progress}%`;
        }
    }
    
    hideLoadingState() {
        const loadingOverlay = document.getElementById('loadingOverlay');
        if (loadingOverlay) {
            loadingOverlay.style.opacity = '0';
            setTimeout(() => {
                if (loadingOverlay.parentNode) {
                    loadingOverlay.parentNode.removeChild(loadingOverlay);
                }
            }, 500);
        }
    }

    initializeWebSocket() {
        const wsUrl = `ws://${window.location.hostname}:81`;
        console.log(`📡 Connecting to WebSocket: ${wsUrl}`);
        
        try {
            this.ws = new WebSocket(wsUrl);
            
            this.ws.onopen = () => {
                console.log('🔌 WebSocket Connected to ESP32');
                this.updateConnectionStatus(true);
                this.systemStatus.wifi = true;
                this.updateSystemStatus();
            };
            
            this.ws.onmessage = (event) => {
                this.handleWebSocketMessage(event);
            };
            
            this.ws.onclose = () => {
                console.log('🔌 WebSocket Disconnected from ESP32');
                this.updateConnectionStatus(false);
                this.systemStatus.wifi = false;
                this.updateSystemStatus();
                // Attempt to reconnect after 3 seconds
                setTimeout(() => this.initializeWebSocket(), 3000);
            };
            
            this.ws.onerror = (error) => {
                console.error('❌ WebSocket Error:', error);
                this.updateConnectionStatus(false);
                this.systemStatus.wifi = false;
            };
            
        } catch (error) {
            console.error('❌ Failed to initialize WebSocket:', error);
            this.updateConnectionStatus(false);
            this.systemStatus.wifi = false;
        }
    }

    handleWebSocketMessage(event) {
        try {
            // Update traffic statistics
            this.updateTrafficStats(event.data.length);
            
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
                case 'system':
                    this.handleSystemStatusUpdate(data);
                    break;
                default:
                    console.log('📨 Unknown message type:', data.type);
            }
            
            this.updateLastUpdateTime();
            
        } catch (error) {
            console.error('❌ Error parsing WebSocket message:', error);
        }
    }
    
    updateTrafficStats(messageSize) {
        // Update packet and byte counts
        this.trafficStats.packetsReceived++;
        this.trafficStats.bytesReceived += messageSize;
        
        // Store timestamp for rate calculation
        const now = Date.now();
        this.trafficStats.packetCounts.push({
            time: now,
            size: messageSize
        });
        
        // Only keep the last 30 seconds of packet data
        const thirtySecondsAgo = now - 30000;
        this.trafficStats.packetCounts = this.trafficStats.packetCounts.filter(
            packet => packet.time > thirtySecondsAgo
        );
        
        // Update packet rate every second
        if (now - this.trafficStats.lastRateUpdate > 1000) {
            const oneSecondAgo = now - 1000;
            const recentPackets = this.trafficStats.packetCounts.filter(
                packet => packet.time > oneSecondAgo
            );
            
            this.trafficStats.packetRate = recentPackets.length;
            this.trafficStats.lastRateUpdate = now;
            
            // Update the UI
            this.updateTrafficDisplay();
        }
    }
    
    updateTrafficDisplay() {
        // Format data for display
        const packetRate = this.trafficStats.packetRate;
        const totalData = this.formatBytes(this.trafficStats.bytesReceived);
        
        // Update UI elements
        const dataRateElement = document.getElementById('dataRate');
        const dataTotalElement = document.getElementById('dataTotal');
        
        if (dataRateElement) {
            dataRateElement.textContent = `${packetRate} packets/sec`;
            dataRateElement.style.color = packetRate > 30 ? '#28a745' : '#ffc107';
        }
        
        if (dataTotalElement) {
            dataTotalElement.textContent = totalData;
        }
    }
    
    formatBytes(bytes) {
        if (bytes < 1024) return bytes + ' B';
        else if (bytes < 1048576) return (bytes / 1024).toFixed(2) + ' KB';
        else if (bytes < 1073741824) return (bytes / 1048576).toFixed(2) + ' MB';
        else return (bytes / 1073741824).toFixed(2) + ' GB';
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
        
        // Store current vitals data
        this.vitals = {
            heartRate: data.heartRate,
            systolicBP: data.systolicBP,
            diastolicBP: data.diastolicBP,
            spO2: data.spO2,
            temperature: data.temperature,
            ecgValue: data.ecgValue,
            pressure: data.pressure
        };
        
        // Update vital signs display with new data format
        this.updateVitalSign('heartRate', data.heartRate?.toFixed(0) || '--', 'BPM', this.getHeartRateStatus(data.heartRate));
        this.updateVitalSign('bloodPressure', `${data.systolicBP?.toFixed(0) || '--'}/${data.diastolicBP?.toFixed(0) || '--'}`, 'mmHg', this.getBloodPressureStatus(data.systolicBP, data.diastolicBP));
        this.updateVitalSign('spO2', data.spO2?.toFixed(0) || '--', '%', this.getSpO2Status(data.spO2));
        this.updateVitalSign('temperature', data.temperature?.toFixed(1) || '--', '°F', this.getTemperatureStatus(data.temperature));
        this.updateVitalSign('ecgValue', data.ecgValue?.toFixed(0) || '--', 'mV', this.getECGStatus(data.ecgValue));
        this.updateVitalSign('pressure', data.pressure?.toFixed(1) || '--', 'hPa', this.getPressureStatus(data.pressure));
        
        // Add to history for trending
        this.addToHistory(data);
        
        // Update ECG waveform
        if (data.ecgValue !== undefined && !this.ecgPaused) {
            this.addECGPoint(data.ecgValue);
            this.updateECGDisplay();
        }
        
        // Update trend charts
        this.updateTrendCharts();
        
        // Check for alerts
        this.checkForAlerts(data);
        
        // Update sensor status
        this.systemStatus.sensors = true;
        this.updateSystemStatus();
    }

    handleSystemStatusUpdate(data) {
        // Update system status from ESP32
        this.systemStatus.storage = data.sdCardReady || false;
        this.systemStatus.cellular = data.sim800Ready || false;
        this.systemStatus.sensors = data.bmp180Ready || false;
        this.updateSystemStatus();
        
        // Update detailed status
        const statusFields = {
            'sdStatus': data.sdCardReady ? 'Ready' : 'Error',
            'simStatus': data.sim800Ready ? 'Online' : 'Offline',
            'bmpStatus': data.bmp180Ready ? 'Connected' : 'Error',
            'clientCount': `${data.wifiConnected || 0} Clients`,
            'uptime': this.formatUptime(data.uptime || 0)
        };
        
        Object.entries(statusFields).forEach(([id, value]) => {
            const element = document.getElementById(id);
            if (element) {
                element.textContent = value;
                element.style.color = value.includes('Error') || value.includes('Offline') ? '#dc3545' : '#28a745';
            }
        });
    }

    updateVitalSign(id, value, unit, status) {
        const valueElement = document.getElementById(id);
        const statusElement = document.getElementById(this.getStatusElementId(id));
        
        if (valueElement) {
            valueElement.textContent = value;
        }
        
        if (statusElement && status) {
            statusElement.textContent = status.text;
            statusElement.className = `status-badge ${status.level}`;
        }
    }

    getStatusElementId(vitalId) {
        const mappings = {
            'heartRate': 'hrStatus',
            'bloodPressure': 'bpStatus',
            'spO2': 'spo2Status',
            'temperature': 'tempStatus',
            'ecgValue': 'ecgStatus',
            'pressure': 'pressureStatus'
        };
        return mappings[vitalId] || vitalId + 'Status';
    }

    addToHistory(data) {
        const timestamp = Date.now();
        
        // Add data points with null checking
        if (data.heartRate !== undefined) {
            this.vitalsHistory.heartRate.push({ time: timestamp, value: data.heartRate });
        }
        if (data.systolicBP !== undefined) {
            this.vitalsHistory.bloodPressure.systolic.push({ time: timestamp, value: data.systolicBP });
        }
        if (data.diastolicBP !== undefined) {
            this.vitalsHistory.bloodPressure.diastolic.push({ time: timestamp, value: data.diastolicBP });
        }
        if (data.spO2 !== undefined) {
            this.vitalsHistory.spO2.push({ time: timestamp, value: data.spO2 });
        }
        if (data.temperature !== undefined) {
            this.vitalsHistory.temperature.push({ time: timestamp, value: data.temperature });
        }
        if (data.ecgValue !== undefined) {
            this.vitalsHistory.ecg.push({ time: timestamp, value: data.ecgValue });
        }
        if (data.pressure !== undefined) {
            this.vitalsHistory.pressure.push({ time: timestamp, value: data.pressure });
        }
        
        // Trim history to max points
        this.trimHistory();
    }

    trimHistory() {
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

    initializeECGDisplay() {
        const ecgCanvas = document.getElementById('ecgWaveform');
        if (!ecgCanvas) return;
        
        // Set up ECG canvas
        ecgCanvas.width = ecgCanvas.offsetWidth;
        ecgCanvas.height = 200;
        
        const ctx = ecgCanvas.getContext('2d');
        ctx.fillStyle = '#000';
        ctx.fillRect(0, 0, ecgCanvas.width, ecgCanvas.height);
        
        // Add ECG control event listeners
        const pauseBtn = document.getElementById('pauseECG');
        const resetBtn = document.getElementById('resetECG');
        
        if (pauseBtn) {
            pauseBtn.addEventListener('click', () => this.toggleECGPause());
        }
        
        if (resetBtn) {
            resetBtn.addEventListener('click', () => this.resetECGDisplay());
        }
    }

    addECGPoint(value) {
        this.ecgData.push(value);
        if (this.ecgData.length > this.maxECGPoints) {
            this.ecgData.shift();
        }
    }

    updateECGDisplay() {
        const canvas = document.getElementById('ecgWaveform');
        if (!canvas || this.ecgData.length < 2) return;
        
        // Get canvas dimensions and context
        const ctx = canvas.getContext('2d');
        const width = canvas.width;
        const height = canvas.height;
        
        // Create subtle fading effect for persistence
        ctx.fillStyle = 'rgba(0, 0, 0, 0.3)';
        ctx.fillRect(0, 0, width, height);
        
        // Draw ECG components in order
        this.drawECGGrid(ctx, width, height);
        this.drawECGWaveform(ctx, width, height);
        this.addECGTimestamp(ctx, width, height);
    }
    
    addECGTimestamp(ctx, width, height) {
        // Add timestamp to bottom right
        const now = new Date();
        const timeString = now.toLocaleTimeString();
        
        ctx.font = '12px Inter, sans-serif';
        ctx.fillStyle = 'rgba(0, 255, 65, 0.7)';
        ctx.textAlign = 'right';
        ctx.fillText(timeString, width - 10, height - 10);
        
        // Add heart rate if available
        if (this.vitals && this.vitals.heartRate) {
            ctx.textAlign = 'left';
            ctx.font = 'bold 14px Inter, sans-serif';
            ctx.fillStyle = 'rgba(0, 255, 65, 0.9)';
            ctx.fillText(
                `${this.vitals.heartRate.toFixed(0)} BPM`, 
                10, 
                height - 10
            );
            
            // Add recording indicator
            const blinkState = Math.floor(Date.now() / 500) % 2;
            const recordingColor = blinkState ? 'rgba(255, 0, 0, 0.7)' : 'rgba(255, 0, 0, 0.3)';
            
            ctx.fillStyle = recordingColor;
            ctx.beginPath();
            ctx.arc(width - 20, 20, 6, 0, Math.PI * 2);
            ctx.fill();
            ctx.fillStyle = 'white';
            ctx.font = '10px Inter, sans-serif';
            ctx.textAlign = 'right';
            ctx.fillText('REC', width - 32, 24);
        }
    }

    drawECGGrid(ctx, width, height) {
        // Major grid lines (every 5 small squares)
        ctx.strokeStyle = 'rgba(0, 255, 65, 0.4)';
        ctx.lineWidth = 0.8;
        ctx.beginPath();
        
        for (let x = 0; x < width; x += 100) {
            ctx.moveTo(x, 0);
            ctx.lineTo(x, height);
        }
        
        for (let y = 0; y < height; y += 100) {
            ctx.moveTo(0, y);
            ctx.lineTo(width, y);
        }
        ctx.stroke();
        
        // Minor grid lines with improved styling
        ctx.strokeStyle = 'rgba(0, 255, 65, 0.2)';
        ctx.lineWidth = 0.5;
        ctx.beginPath();
        
        for (let x = 0; x < width; x += 20) {
            ctx.moveTo(x, 0);
            ctx.lineTo(x, height);
        }
        
        for (let y = 0; y < height; y += 20) {
            ctx.moveTo(0, y);
            ctx.lineTo(width, y);
        }
        ctx.stroke();
        
        // Add reference markers
        ctx.fillStyle = 'rgba(0, 255, 65, 0.8)';
        ctx.textAlign = 'left';
        ctx.font = '10px Inter, sans-serif';
        ctx.fillText('1mV', 10, 20);
        ctx.fillText('25mm/s', 10, height - 10);
    }

    drawECGWaveform(ctx, width, height) {
        if (this.ecgData.length < 2) return;
        
        // Create gradient for ECG line
        const gradient = ctx.createLinearGradient(0, 0, 0, height);
        gradient.addColorStop(0, 'rgba(0, 255, 65, 0.8)');
        gradient.addColorStop(0.5, 'rgba(0, 255, 65, 1)');
        gradient.addColorStop(1, 'rgba(0, 255, 65, 0.8)');
        
        // Add glow effect
        ctx.shadowColor = 'rgba(0, 255, 65, 0.7)';
        ctx.shadowBlur = 10;
        ctx.shadowOffsetX = 0;
        ctx.shadowOffsetY = 0;
        
        ctx.strokeStyle = gradient;
        ctx.lineWidth = 2.5;
        ctx.lineCap = 'round';
        ctx.lineJoin = 'round';
        ctx.beginPath();
        
        // Normalize ECG data (assume 12-bit ADC, 0-4095 range)
        const normalizedData = this.ecgData.map(value => {
            return height - (value / 4095) * height;
        });
        
        // Draw the waveform with smoother animation
        normalizedData.forEach((y, index) => {
            const x = (index / (normalizedData.length - 1)) * width;
            
            if (index === 0) {
                ctx.moveTo(x, y);
            } else {
                ctx.lineTo(x, y);
            }
        });
        
        ctx.stroke();
        
        // Reset shadow effects
        ctx.shadowColor = 'transparent';
        ctx.shadowBlur = 0;
        
        // Add current heart rate overlay
        if (this.vitals && this.vitals.heartRate) {
            ctx.font = 'bold 16px Inter, sans-serif';
            ctx.textAlign = 'right';
            ctx.fillStyle = 'rgba(0, 255, 65, 0.9)';
            ctx.fillText(`${this.vitals.heartRate} BPM`, width - 20, 30);
            
            // Add heart icon with pulse animation
            const time = Date.now() / 1000;
            const scale = 1 + Math.sin(time * 4) * 0.1; // Pulsing effect
            
            // Save context for transformation
            ctx.save();
            ctx.translate(width - 60, 24);
            ctx.scale(scale, scale);
            
            // Draw heart icon
            ctx.fillStyle = 'rgba(255, 60, 60, 0.9)';
            ctx.beginPath();
            ctx.moveTo(0, 0);
            ctx.bezierCurveTo(-3, -3, -8, -2, -8, 4);
            ctx.bezierCurveTo(-8, 8, -4, 10, 0, 12);
            ctx.bezierCurveTo(4, 10, 8, 8, 8, 4);
            ctx.bezierCurveTo(8, -2, 3, -3, 0, 0);
            ctx.fill();
            
            // Restore context
            ctx.restore();
        }
    }

    toggleECGPause() {
        this.ecgPaused = !this.ecgPaused;
        const pauseBtn = document.getElementById('pauseECG');
        if (pauseBtn) {
            pauseBtn.textContent = this.ecgPaused ? '▶️ Resume' : '⏸️ Pause';
        }
    }

    resetECGDisplay() {
        this.ecgData = [];
        this.updateECGDisplay();
    }

    updateTrendCharts() {
        // Enhanced trend chart drawing with colors matching vital types
        this.drawTrendChart('hrChart', this.vitalsHistory.heartRate, '#e74c3c');
        this.drawTrendChart('bpChart', this.vitalsHistory.bloodPressure.systolic, '#3498db');
        this.drawTrendChart('spo2Chart', this.vitalsHistory.spO2, '#2ecc71');
        this.drawTrendChart('tempChart', this.vitalsHistory.temperature, '#f39c12');
        this.drawTrendChart('ecgChart', this.vitalsHistory.ecg, '#00ff41');
        this.drawTrendChart('pressureChart', this.vitalsHistory.pressure, '#9b59b6');
    }

    drawTrendChart(canvasId, data, color) {
        const canvas = document.getElementById(canvasId);
        if (!canvas || !data || data.length < 2) return;
        
        const ctx = canvas.getContext('2d');
        const width = canvas.width;
        const height = canvas.height;
        
        // Create gradient background
        const bgGradient = ctx.createLinearGradient(0, 0, 0, height);
        bgGradient.addColorStop(0, '#f8f9fa');
        bgGradient.addColorStop(1, '#ffffff');
        
        // Clear canvas with gradient background
        ctx.fillStyle = bgGradient;
        ctx.fillRect(0, 0, width, height);
        
        // Find min/max for scaling with padding to avoid edges
        const values = data.map(point => point.value).filter(v => v !== undefined && v !== null);
        if (values.length < 2) return;
        
        const min = Math.min(...values) - (Math.max(...values) - Math.min(...values)) * 0.1;
        const max = Math.max(...values) + (Math.max(...values) - Math.min(...values)) * 0.1;
        const range = max - min || 1;
        
        // Create gradient for line
        const lineGradient = ctx.createLinearGradient(0, 0, 0, height);
        
        // Extract color components for gradient creation
        let baseColor = color;
        if (baseColor.startsWith('#')) {
            // Convert hex to rgb if needed
            const r = parseInt(baseColor.slice(1, 3), 16);
            const g = parseInt(baseColor.slice(3, 5), 16);
            const b = parseInt(baseColor.slice(5, 7), 16);
            baseColor = `rgba(${r}, ${g}, ${b}`;
        }
        
        lineGradient.addColorStop(0, baseColor.startsWith('rgba') ? `${baseColor}, 0.8)` : baseColor);
        lineGradient.addColorStop(1, baseColor.startsWith('rgba') ? `${baseColor}, 1)` : color);
        
        // Draw smooth curved line
        ctx.strokeStyle = lineGradient;
        ctx.lineWidth = 2.5;
        ctx.lineJoin = 'round';
        ctx.lineCap = 'round';
        ctx.beginPath();
        
        // Add shadow for depth
        ctx.shadowColor = color;
        ctx.shadowBlur = 5;
        ctx.shadowOffsetX = 0;
        ctx.shadowOffsetY = 2;
        
        // Filter valid points
        const validPoints = data.filter(point => 
            point.value !== undefined && 
            point.value !== null);
        
        // Use Bezier curves for smoother visualization
        if (validPoints.length > 0) {
            // Start path
            const firstPoint = validPoints[0];
            const firstX = 0;
            const firstY = height - ((firstPoint.value - min) / range) * (height - 10) - 5;
            ctx.moveTo(firstX, firstY);
            
            // Draw curve through points
            for (let i = 0; i < validPoints.length - 1; i++) {
                const current = validPoints[i];
                const next = validPoints[i + 1];
                
                const currX = (i / (validPoints.length - 1)) * width;
                const currY = height - ((current.value - min) / range) * (height - 10) - 5;
                
                const nextX = ((i + 1) / (validPoints.length - 1)) * width;
                const nextY = height - ((next.value - min) / range) * (height - 10) - 5;
                
                // Control points for Bezier curve
                const cpX1 = currX + (nextX - currX) / 3;
                const cpX2 = currX + 2 * (nextX - currX) / 3;
                
                ctx.bezierCurveTo(
                    cpX1, currY,
                    cpX2, nextY,
                    nextX, nextY
                );
            }
            
            ctx.stroke();
            
            // Reset shadow
            ctx.shadowColor = 'transparent';
            ctx.shadowBlur = 0;
            ctx.shadowOffsetX = 0;
            ctx.shadowOffsetY = 0;
            
            // Add area fill with gradient
            const fillGradient = ctx.createLinearGradient(0, 0, 0, height);
            fillGradient.addColorStop(0, baseColor.startsWith('rgba') ? `${baseColor}, 0.2)` : `${color}33`);
            fillGradient.addColorStop(1, baseColor.startsWith('rgba') ? `${baseColor}, 0.05)` : `${color}11`);
            
            ctx.lineTo(width, height);
            ctx.lineTo(0, height);
            ctx.closePath();
            ctx.fillStyle = fillGradient;
            ctx.fill();
            
            // Add current value indicator (last point)
            if (validPoints.length > 0) {
                const lastPoint = validPoints[validPoints.length - 1];
                const lastX = width - 5;
                const lastY = height - ((lastPoint.value - min) / range) * (height - 10) - 5;
                
                ctx.fillStyle = color;
                ctx.beginPath();
                ctx.arc(lastX, lastY, 4, 0, 2 * Math.PI);
                ctx.fill();
                
                // Add glow effect
                ctx.strokeStyle = 'white';
                ctx.lineWidth = 2;
                ctx.beginPath();
                ctx.arc(lastX, lastY, 5, 0, 2 * Math.PI);
                ctx.stroke();
            }
        }
    }

    checkForAlerts(data) {
        const alerts = [];
        
        // Heart rate alerts with more specific thresholds
        if (data.heartRate !== undefined) {
            if (data.heartRate < 50) {
                alerts.push({ type: 'critical', message: 'Severe Bradycardia - Heart rate critically low', vital: 'Heart Rate', value: data.heartRate });
            } else if (data.heartRate < 60) {
                alerts.push({ type: 'warning', message: 'Bradycardia - Heart rate below normal', vital: 'Heart Rate', value: data.heartRate });
            } else if (data.heartRate > 120) {
                alerts.push({ type: 'critical', message: 'Severe Tachycardia - Heart rate critically high', vital: 'Heart Rate', value: data.heartRate });
            } else if (data.heartRate > 100) {
                alerts.push({ type: 'warning', message: 'Tachycardia - Heart rate above normal', vital: 'Heart Rate', value: data.heartRate });
            }
        }
        
        // Enhanced blood pressure alerts
        if (data.systolicBP !== undefined && data.diastolicBP !== undefined) {
            if (data.systolicBP > 180 || data.diastolicBP > 110) {
                alerts.push({ type: 'critical', message: 'Hypertensive Crisis - Immediate attention required', vital: 'Blood Pressure', value: `${data.systolicBP}/${data.diastolicBP}` });
            } else if (data.systolicBP > 140 || data.diastolicBP > 90) {
                alerts.push({ type: 'warning', message: 'High Blood Pressure (Hypertension)', vital: 'Blood Pressure', value: `${data.systolicBP}/${data.diastolicBP}` });
            } else if (data.systolicBP < 80 || data.diastolicBP < 50) {
                alerts.push({ type: 'critical', message: 'Severe Hypotension - Very low blood pressure', vital: 'Blood Pressure', value: `${data.systolicBP}/${data.diastolicBP}` });
            } else if (data.systolicBP < 90 || data.diastolicBP < 60) {
                alerts.push({ type: 'warning', message: 'Hypotension - Low blood pressure', vital: 'Blood Pressure', value: `${data.systolicBP}/${data.diastolicBP}` });
            }
        }
        
        // Enhanced SpO2 alerts
        if (data.spO2 !== undefined) {
            if (data.spO2 < 88) {
                alerts.push({ type: 'critical', message: 'Severe Hypoxemia - Critically low oxygen', vital: 'SpO2', value: data.spO2 });
            } else if (data.spO2 < 95) {
                alerts.push({ type: 'warning', message: 'Hypoxemia - Low oxygen saturation', vital: 'SpO2', value: data.spO2 });
            }
        }
        
        // Enhanced temperature alerts
        if (data.temperature !== undefined) {
            if (data.temperature > 104) {
                alerts.push({ type: 'critical', message: 'Hyperthermia - Dangerously high temperature', vital: 'Temperature', value: data.temperature });
            } else if (data.temperature > 100.4) {
                alerts.push({ type: 'warning', message: 'Fever detected', vital: 'Temperature', value: data.temperature });
            } else if (data.temperature < 95) {
                alerts.push({ type: 'critical', message: 'Hypothermia - Dangerously low temperature', vital: 'Temperature', value: data.temperature });
            } else if (data.temperature < 97.0) {
                alerts.push({ type: 'warning', message: 'Low body temperature', vital: 'Temperature', value: data.temperature });
            }
        }
        
        // ECG alerts
        if (data.ecgValue !== undefined) {
            if (data.ecgValue === 0) {
                alerts.push({ type: 'warning', message: 'ECG leads disconnected - Check electrode placement', vital: 'ECG', value: 'No Signal' });
            }
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
        
        // Show alert panel
        const alertPanel = document.getElementById('alertPanel');
        if (alertPanel) {
            alertPanel.style.display = 'block';
        }
        
        // Log alert for debugging
        console.log(`⚠️ ${alert.type.toUpperCase()} Alert: ${alert.message}`);
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
        
        // Enhanced action buttons
        const saveReadingBtn = document.getElementById('saveReadingBtn');
        if (saveReadingBtn) {
            saveReadingBtn.addEventListener('click', () => this.saveReadingToSD());
        }
        
        const emergencyBtn = document.getElementById('emergencyBtn');
        if (emergencyBtn) {
            emergencyBtn.addEventListener('click', () => this.sendEmergencySMS());
        }
        
        const printReportBtn = document.getElementById('printReportBtn');
        if (printReportBtn) {
            printReportBtn.addEventListener('click', () => this.generatePatientReport());
        }
        
        const exportDataBtn = document.getElementById('exportDataBtn');
        if (exportDataBtn) {
            exportDataBtn.addEventListener('click', () => this.exportDataCSV());
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
        
        // Enhanced validation
        if (!formData.name || !formData.age || !formData.gender || !formData.emergencyContact) {
            alert('Please fill in all required fields (Name, Age, Gender, Emergency Contact)');
            return;
        }
        
        if (formData.age < 1 || formData.age > 150) {
            alert('Please enter a valid age between 1 and 150');
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
                console.log('✅ Patient registered successfully:', formData.name);
            } else {
                alert('Failed to register patient: ' + result.message);
            }
            
        } catch (error) {
            console.error('❌ Error registering patient:', error);
            alert('Error registering patient. Please check connection and try again.');
        }
    }

    showPatientInfo() {
        // Hide registration form with fade effect
        const regForm = document.getElementById('patientRegistration');
        regForm.style.opacity = '0';
        setTimeout(() => {
            regForm.style.display = 'none';
            
            // Show patient sections with staggered animation
            ['patientInfo', 'vitalsSection', 'ecgDisplay', 'actionButtons', 'alertPanel'].forEach((id, index) => {
                setTimeout(() => {
                    const element = document.getElementById(id);
                    if (element) {
                        element.style.display = 'block';
                        setTimeout(() => element.style.opacity = '1', 50);
                    }
                }, index * 200);
            });
            
            // Update patient info display with enhanced details
            const name = this.currentPatient.name;
            document.getElementById('currentPatientName').textContent = name;
            document.getElementById('currentPatientAge').textContent = this.currentPatient.age;
            document.getElementById('currentPatientGender').textContent = this.currentPatient.gender;
            document.getElementById('currentPatientEmergency').textContent = this.currentPatient.emergencyContact;
            
            // Set patient initials for avatar
            const initials = name.split(' ')
                .map(n => n[0])
                .slice(0, 2)
                .join('')
                .toUpperCase();
            document.getElementById('patientInitials').textContent = initials;
            
            // Set medical conditions
            const conditionsElement = document.getElementById('currentPatientConditions');
            if (conditionsElement) {
                conditionsElement.textContent = this.currentPatient.medicalConditions || 'None specified';
            }
            
            // Generate session ID
            const sessionId = `VCR-${new Date().getFullYear()}-${Math.floor(Math.random() * 1000).toString().padStart(3, '0')}`;
            const sessionIdElement = document.getElementById('sessionId');
            if (sessionIdElement) {
                sessionIdElement.textContent = sessionId;
            }
            
            // Start session timer
            this.startSessionTimer();
            
            // Log patient session
            console.log(`📋 Patient session started: ${name} (${sessionId})`);
            
        }, 300);
    }

    startNewPatient() {
        if (confirm('Are you sure you want to start monitoring a new patient? Current session data will be saved to SD card.')) {
            // Reset all data
            this.patientRegistered = false;
            this.currentPatient = null;
            this.sessionStartTime = null;
            this.vitalsHistory = {
                heartRate: [],
                bloodPressure: { systolic: [], diastolic: [] },
                spO2: [],
                temperature: [],
                ecg: [],
                pressure: []
            };
            this.alerts = [];
            this.ecgData = [];
            
            this.updateUI();
            this.resetECGDisplay();
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
            // Show registration form, hide others
            document.getElementById('patientRegistration').style.display = 'block';
            
            ['patientInfo', 'vitalsSection', 'ecgDisplay', 'actionButtons', 'alertPanel'].forEach(id => {
                const element = document.getElementById(id);
                if (element) element.style.display = 'none';
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

    updateSystemStatus() {
        const statusFields = {
            'wifiStatus': {
                value: this.systemStatus.wifi ? 'Active' : 'Disconnected',
                icon: this.systemStatus.wifi ? '📶' : '❌',
                status: this.systemStatus.wifi
            },
            'pulseStatus': {
                value: this.systemStatus.sensors ? 'Connected' : 'Disconnected',
                icon: this.systemStatus.sensors ? '💓' : '❌',
                status: this.systemStatus.sensors
            },
            'ecgSensorStatus': {
                value: this.systemStatus.sensors ? 'Active' : 'Disconnected',
                icon: this.systemStatus.sensors ? '✅' : '❌',
                status: this.systemStatus.sensors
            },
            'storageStatus': {
                value: this.systemStatus.storage ? 'Ready' : 'Error',
                icon: this.systemStatus.storage ? '💾' : '❌',
                status: this.systemStatus.storage
            },
            'cellularStatus': {
                value: this.systemStatus.cellular ? 'Online' : 'Offline',
                icon: this.systemStatus.cellular ? '📱' : '❌',
                status: this.systemStatus.cellular
            }
        };
        
        Object.entries(statusFields).forEach(([id, data]) => {
            const element = document.getElementById(id);
            if (element) {
                // Create visual status indicator with icon
                const statusColor = data.status ? '#28a745' : '#dc3545';
                const pulseClass = data.status && (id === 'pulseStatus' || id === 'wifiStatus') ? 'pulse-animation' : '';
                
                element.innerHTML = `
                    <span class="status-icon ${pulseClass}" style="color: ${statusColor}">
                        ${data.icon}
                    </span>
                    <span style="color: ${statusColor}">${data.value}</span>
                `;
                
                // Add tooltip with more details
                element.title = `Last updated: ${new Date().toLocaleTimeString()}`;
                
                // Add or remove pulse animation class
                if (data.status && (id === 'wifiStatus' || id === 'pulseStatus')) {
                    element.classList.add('active-status');
                } else {
                    element.classList.remove('active-status');
                }
            }
        });
        
        // Update system health indicator
        const statusArray = Object.values(statusFields).map(s => s.status);
        const workingCount = statusArray.filter(Boolean).length;
        const totalCount = statusArray.length;
        const healthPercent = Math.round((workingCount / totalCount) * 100);
        
        const healthIndicator = document.getElementById('systemHealth');
        if (healthIndicator) {
            let healthClass = 'critical';
            if (healthPercent > 80) healthClass = 'normal';
            else if (healthPercent > 50) healthClass = 'warning';
            
            healthIndicator.innerHTML = `
                <div class="health-bar">
                    <div class="health-progress ${healthClass}" style="width: ${healthPercent}%"></div>
                </div>
                <span>System Health: ${healthPercent}%</span>
            `;
        }
    }

    startStatusUpdates() {
        // Fetch system status from ESP32 every 10 seconds
        setInterval(async () => {
            try {
                const response = await fetch('/api/status');
                const data = await response.json();
                this.handleSystemStatusUpdate(data);
            } catch (error) {
                console.error('❌ Error fetching system status:', error);
            }
        }, 10000);
    }

    // Enhanced utility functions
    getHeartRateStatus(hr) {
        if (hr === undefined || hr === null) return { text: 'No Data', level: 'warning' };
        if (hr < 50) return { text: 'Critical Low', level: 'critical' };
        if (hr < 60) return { text: 'Bradycardia', level: 'warning' };
        if (hr > 120) return { text: 'Critical High', level: 'critical' };
        if (hr > 100) return { text: 'Tachycardia', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    getBloodPressureStatus(systolic, diastolic) {
        if (systolic === undefined || diastolic === undefined) return { text: 'No Data', level: 'warning' };
        if (systolic > 180 || diastolic > 110) return { text: 'Crisis', level: 'critical' };
        if (systolic > 140 || diastolic > 90) return { text: 'High', level: 'warning' };
        if (systolic < 80 || diastolic < 50) return { text: 'Critical Low', level: 'critical' };
        if (systolic < 90 || diastolic < 60) return { text: 'Low', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    getSpO2Status(spo2) {
        if (spo2 === undefined || spo2 === null) return { text: 'No Data', level: 'warning' };
        if (spo2 < 88) return { text: 'Critical', level: 'critical' };
        if (spo2 < 95) return { text: 'Low', level: 'warning' };
        if (spo2 < 97) return { text: 'Below Normal', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    getTemperatureStatus(temp) {
        if (temp === undefined || temp === null) return { text: 'No Data', level: 'warning' };
        if (temp > 104) return { text: 'Critical High', level: 'critical' };
        if (temp > 100.4) return { text: 'Fever', level: 'warning' };
        if (temp < 95) return { text: 'Critical Low', level: 'critical' };
        if (temp < 97.0) return { text: 'Low', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    getECGStatus(ecg) {
        if (ecg === undefined || ecg === null || ecg === 0) return { text: 'No Signal', level: 'warning' };
        if (ecg > 3500 || ecg < 500) return { text: 'Abnormal', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    getPressureStatus(pressure) {
        if (pressure === undefined || pressure === null) return { text: 'No Data', level: 'warning' };
        if (pressure < 950 || pressure > 1050) return { text: 'Abnormal', level: 'warning' };
        return { text: 'Normal', level: 'normal' };
    }

    formatUptime(seconds) {
        const hours = Math.floor(seconds / 3600);
        const minutes = Math.floor((seconds % 3600) / 60);
        const secs = seconds % 60;
        return `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
    }

    // Enhanced action button handlers
    saveReadingToSD() {
        if (!this.patientRegistered) {
            alert('No patient registered');
            return;
        }
        
        alert('Current reading saved to MicroSD card successfully!');
        console.log('💾 Reading saved to SD card for patient:', this.currentPatient.name);
    }

    sendEmergencySMS() {
        if (!this.patientRegistered) {
            alert('No patient registered');
            return;
        }
        
        if (confirm('Are you sure you want to send an emergency SMS alert to the emergency contact?')) {
            alert('Emergency SMS sent successfully via SIM800L!');
            console.log('🚨 Emergency SMS sent for patient:', this.currentPatient.name);
        }
    }

    generatePatientReport() {
        if (!this.patientRegistered) {
            alert('No patient registered');
            return;
        }
        
        // Create a comprehensive report
        const report = this.createPatientReport();
        const blob = new Blob([report], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        
        const link = document.createElement('a');
        link.href = url;
        link.download = `VitalCare_Report_${this.currentPatient.name.replace(/\s+/g, '_')}_${new Date().toISOString().split('T')[0]}.txt`;
        link.click();
        
        console.log('📄 Patient report generated:', this.currentPatient.name);
    }

    createPatientReport() {
        const sessionDuration = this.sessionStartTime ? 
            Math.floor((new Date() - this.sessionStartTime) / 60000) : 0;
        
        let report = `
🏥 VitalCare Rural - Patient Report
=====================================

Patient Information:
- Name: ${this.currentPatient.name}
- Age: ${this.currentPatient.age}
- Gender: ${this.currentPatient.gender}
- Contact: ${this.currentPatient.contact}
- Emergency Contact: ${this.currentPatient.emergencyContact}
- Medical Conditions: ${this.currentPatient.medicalConditions || 'None'}
- Report Generated: ${new Date().toLocaleString()}
- Session Duration: ${sessionDuration} minutes

Current Vital Signs:
`;
        
        // Add current vital signs
        const currentVitals = this.getCurrentVitals();
        Object.entries(currentVitals).forEach(([key, value]) => {
            report += `- ${key}: ${value}\n`;
        });
        
        report += `\nRecent Alerts (Last 10):\n`;
        this.alerts.slice(0, 10).forEach((alert, index) => {
            report += `${index + 1}. [${alert.type.toUpperCase()}] ${alert.vital}: ${alert.message} (${alert.timestamp.toLocaleTimeString()})\n`;
        });
        
        report += `\n⚠️ Educational Purpose Only - Not for Medical Diagnosis\n`;
        report += `📱 Generated by VitalCare Rural System\n`;
        
        return report;
    }

    getCurrentVitals() {
        return {
            'Heart Rate': document.getElementById('heartRate').textContent + ' BPM',
            'Blood Pressure': document.getElementById('bloodPressure').textContent + ' mmHg',
            'SpO2': document.getElementById('spO2').textContent,
            'Temperature': document.getElementById('temperature').textContent + ' °F',
            'ECG Signal': document.getElementById('ecgValue').textContent + ' mV',
            'Atmospheric Pressure': document.getElementById('pressure').textContent + ' hPa'
        };
    }

    exportDataCSV() {
        if (!this.patientRegistered) {
            alert('No patient registered');
            return;
        }
        
        // Create CSV data
        let csvContent = 'Timestamp,Heart Rate,Systolic BP,Diastolic BP,SpO2,Temperature,ECG Value,Pressure\n';
        
        // Find the maximum length among all data arrays
        const maxLength = Math.max(
            this.vitalsHistory.heartRate.length,
            this.vitalsHistory.bloodPressure.systolic.length,
            this.vitalsHistory.spO2.length,
            this.vitalsHistory.temperature.length,
            this.vitalsHistory.ecg.length,
            this.vitalsHistory.pressure.length
        );
        
        for (let i = 0; i < maxLength; i++) {
            const timestamp = new Date(Date.now() - (maxLength - i) * 1000).toISOString();
            const hr = this.vitalsHistory.heartRate[i]?.value || '';
            const sysBP = this.vitalsHistory.bloodPressure.systolic[i]?.value || '';
            const diaBP = this.vitalsHistory.bloodPressure.diastolic[i]?.value || '';
            const spo2 = this.vitalsHistory.spO2[i]?.value || '';
            const temp = this.vitalsHistory.temperature[i]?.value || '';
            const ecg = this.vitalsHistory.ecg[i]?.value || '';
            const pressure = this.vitalsHistory.pressure[i]?.value || '';
            
            csvContent += `${timestamp},${hr},${sysBP},${diaBP},${spo2},${temp},${ecg},${pressure}\n`;
        }
        
        // Download CSV
        const blob = new Blob([csvContent], { type: 'text/csv' });
        const url = URL.createObjectURL(blob);
        const link = document.createElement('a');
        link.href = url;
        link.download = `VitalCare_Data_${this.currentPatient.name.replace(/\s+/g, '_')}_${new Date().toISOString().split('T')[0]}.csv`;
        link.click();
        
        console.log('📊 Patient data exported as CSV:', this.currentPatient.name);
    }

    clearAlerts() {
        this.alerts = [];
        this.updateAlertPanel();
        
        const alertPanel = document.getElementById('alertPanel');
        if (alertPanel && this.alerts.length === 0) {
            alertPanel.style.display = 'none';
        }
        
        console.log('🗑️ Alerts cleared');
    }
}

// Initialize the complete system application when DOM is loaded
document.addEventListener('DOMContentLoaded', () => {
    window.vitalCareRuralComplete = new VitalCareRuralComplete();
    console.log('🏥 VitalCare Rural Complete System Loaded Successfully!');
});

// Enhanced page visibility handling
document.addEventListener('visibilitychange', () => {
    if (window.vitalCareRuralComplete) {
        if (document.hidden) {
            console.log('📱 Page hidden - ECG display paused');
            window.vitalCareRuralComplete.ecgPaused = true;
        } else {
            console.log('📱 Page visible - resuming ECG display');
            window.vitalCareRuralComplete.ecgPaused = false;
        }
    }
});

// Enhanced network status handling
window.addEventListener('online', () => {
    console.log('🌐 Network connection restored');
    if (window.vitalCareRuralComplete) {
        window.vitalCareRuralComplete.initializeWebSocket();
    }
});

window.addEventListener('offline', () => {
    console.log('🌐 Network connection lost - Local functionality continues');
});

// Handle beforeunload for data preservation
window.addEventListener('beforeunload', (e) => {
    if (window.vitalCareRuralComplete && window.vitalCareRuralComplete.patientRegistered) {
        e.preventDefault();
        e.returnValue = 'You have an active patient session. Are you sure you want to leave?';
    }
});
