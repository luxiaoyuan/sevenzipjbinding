package net.sf.sevenzip.simple.impl;

import net.sf.sevenzip.ISevenZipInArchive;
import net.sf.sevenzip.SevenZipException;
import net.sf.sevenzip.simple.ISimpleInArchive;
import net.sf.sevenzip.simple.ISimpleInArchiveItem;

public class SimpleInArchiveImpl implements ISimpleInArchive {
	private final ISevenZipInArchive sevenZipInArchive;
	private boolean wasClosed = false;

	public SimpleInArchiveImpl(ISevenZipInArchive sevenZipInArchive) {
		this.sevenZipInArchive = sevenZipInArchive;
	}

	@Override
	public void close() throws SevenZipException {
		sevenZipInArchive.close();
		wasClosed = true;
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public ISimpleInArchiveItem[] getArchiveItems() throws SevenZipException {
		ISimpleInArchiveItem[] result = new ISimpleInArchiveItem[getNumberOfItems()];
		for (int i = 0; i < result.length; i++) {
			result[i] = new SimpleInArchiveItemImpl(this, i);
		}
		return result;
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int getNumberOfItems() throws SevenZipException {
		return testAndGetSafeSevenZipInArchive().getNumberOfItems();
	}

	/**
	 * Tests, if 7-Zip In archive interface can be accessed safely.
	 * 
	 * @return 7-Zip In archive interface
	 * @throws SevenZipException
	 *             archive can't be accessed any more
	 */
	public ISevenZipInArchive testAndGetSafeSevenZipInArchive()
			throws SevenZipException {
		if (wasClosed) {
			throw new SevenZipException("Archive was closed");
		}
		return sevenZipInArchive;
	}
}