package de.psi.telco.sumoplayer.util;

public interface Vector {
	public double getX();
	public double getY();
	public Vector add(Vector b);
	public Vector sub(Vector b);
	public Vector scale(double scalar);
}
