package model;

import java.util.Date;

import heatingbackend.HeatingbackendApplication;

public class MeasuredValue {
	
	private String type;
	private Double value; 
	private Date date;
	
	public MeasuredValue(String type, double value) {
		this.type = type;
		this.value = value;
		this.date = new Date();
	}
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public Double getValue() {
		return value;
	}
	public void setValue(Double value) {
		this.value = value;
	}
	public Date getDate() {
		return date;
	}
	public void setDate(Date date) {
		this.date = date;
	}
	
	
	 @Override
	public String toString() {
		// TODO Auto-generated method stub
		return "{\"value\":" + value +",\"date\":" + date.getTime() + ",\"type\":\"" + type.toString() + "\"}";
	}
	
}
